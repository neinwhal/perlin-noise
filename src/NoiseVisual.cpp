#include "glhelper.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Noise.hpp"

#include "camera.hpp"

constexpr glm::vec3 cam_start_pos{ 0.0f, 1.0f, 10.0f };

camera main_camera(
    camera_type::persp,
    cam_start_pos
);

void PerlinNoise::load() {
    add_shader_programs("terrain", "shaders/terrain.vert", "shaders/terrain.frag");
}

void PerlinNoise::init() {
    GeneratePerlinNoise();
    GeneratePerlinNoiseWithGradient();

    GenerateDLATerrain(0);

    // Generate terrain for both planes
    GeneratePerlinNoiseTerrain(perlinNoise, vertices[PERLIN_NOISE], indices[PERLIN_NOISE]);
    GeneratePerlinNoiseTerrain(perlinNoiseWithGradient, vertices[GRAIDENT_NOISE], indices[GRAIDENT_NOISE]);


    perlinNoise.reserve(outputWidth * outputDepth);
    perlinNoiseWithGradient.reserve(outputWidth * outputDepth);

    // Set up VAOs, VBOs, and EBOs for both planes
    glGenVertexArrays(3, vao);
    glGenBuffers(3, vbo);
    glGenBuffers(3, ebo);

    for (int i = 0; i < 3; ++i) {
        glBindVertexArray(vao[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
        glBufferData(GL_ARRAY_BUFFER, vertices[i].size() * sizeof(Vertex), vertices[i].data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(unsigned int), indices[i].data(), GL_STATIC_DRAW);
        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        // Color attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glBindVertexArray(0);
    }
}

void PerlinNoise::update(double dt) {
    elapsedTime += dt;
    glViewport(0, 0, GLHelper::width, GLHelper::height);
    // handle movement controls
    control(static_cast<float>(dt));
    // ImGui Stuff
    gui();
    // apply wireframe
    shdrpgms["terrain"].use();
    GLuint uniform_uApplyLighting = glGetUniformLocation(shdrpgms["terrain"].get_handle(), "uApplyLighting");
    if (visualization_primitive == 1) { // draw lines
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUniform1i(uniform_uApplyLighting, false); // disable lighting for lines
    }
    else if (visualization_primitive == 2) { // draw points
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glUniform1i(uniform_uApplyLighting, false); // disable lighting for points
        glPointSize(2.f);
    }
    else { // draw planes
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glUniform1i(uniform_uApplyLighting, true); // enable lighting for planes
    }
    shdrpgms["terrain"].unuse();

    // Call draw function to draw the scene
    draw();
}

void PerlinNoise::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    shdrpgms["terrain"].use();

    // Set uniforms
    GLuint uniform_uLightPos = glGetUniformLocation(shdrpgms["terrain"].get_handle(), "uLightPos");
    glUniform3f(uniform_uLightPos, light_position.x, light_position.y, light_position.z);

    // Draw terrain
    glm::mat4 M = glm::mat4(1.0f);
    glm::mat4 VP = main_camera.get_look_at();
    glm::mat4 PP = main_camera.get_projection(static_cast<float>(GLHelper::width) / static_cast<float>(GLHelper::height));

    glm::mat4 MV = VP * M;
    glm::mat4 mvp = PP * MV;

    GLuint uniform_mvpMatrix = glGetUniformLocation(shdrpgms["terrain"].get_handle(), "mvpMatrix");
    glUniformMatrix4fv(uniform_mvpMatrix, 1, GL_FALSE, glm::value_ptr(mvp));
    GLuint uniform_uViewPos = glGetUniformLocation(shdrpgms["terrain"].get_handle(), "uViewPos");
    glUniform3f(uniform_uViewPos, main_camera.get_position().x, main_camera.get_position().y, main_camera.get_position().z);

    // Draw both noise terrains
    if (useDLA) {
        glBindVertexArray(vao[DLA_NOISE]);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices[DLA_NOISE].size()), GL_UNSIGNED_INT, 0);
    }
    else {
        for (int i = 0; i < 2; ++i) {
            M = glm::mat4(1.0f);

            if (i == 0) { // pure perlin noise
                M = glm::translate(M, glm::vec3(-15.0f, 0.0f, 0.0f));
                M = glm::scale(M, glm::vec3{ 20.f, 20.f, 20.f });
            }
            if (i == 1) { // perlin noise + gradient
                M = glm::translate(M, glm::vec3(15.0f, 0.0f, 0.0f));
                M = glm::scale(M, glm::vec3{ 20.f, 20.f, 20.f });
            }

            MV = VP * M;
            mvp = PP * MV;
            glUniformMatrix4fv(uniform_mvpMatrix, 1, GL_FALSE, glm::value_ptr(mvp));
            glBindVertexArray(vao[i]);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices[i].size()), GL_UNSIGNED_INT, 0);
        }
    }

    glBindVertexArray(0);
    shdrpgms["terrain"].unuse();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void PerlinNoise::cleanup() {
    glDeleteVertexArrays(3, vao);
    glDeleteBuffers(3, vbo);
    glDeleteBuffers(3, ebo);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void PerlinNoise::RegenerateNoise() {
    if (useDLA) {
        // DLA ///////////////////////////////////////////////////
        GenerateDLATerrain(1);  
        GenerateTerrainDLA();  
    }
    else {
        // PERLIN ///////////////////////////////////////////////
        GeneratePerlinNoise();
        GeneratePerlinNoiseWithGradient();
        GeneratePerlinNoiseTerrain(perlinNoise, vertices[PERLIN_NOISE], indices[PERLIN_NOISE]);
        GeneratePerlinNoiseTerrain(perlinNoiseWithGradient, vertices[GRAIDENT_NOISE], indices[GRAIDENT_NOISE]);
    }

    // Update GPU buffers
    int numTerrainTypes = useDLA ? 1 : 2;
    for (int i = 0; i < numTerrainTypes; ++i) {
        int terrainType = useDLA ? DLA_NOISE : i;
        glBindBuffer(GL_ARRAY_BUFFER, vbo[terrainType]);
        glBufferData(GL_ARRAY_BUFFER, vertices[terrainType].size() * sizeof(Vertex), vertices[terrainType].data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[terrainType]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[terrainType].size() * sizeof(unsigned int), indices[terrainType].data(), GL_STATIC_DRAW);
    }
}

