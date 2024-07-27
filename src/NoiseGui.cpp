
#include "Noise.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "camera.hpp"

void PerlinNoise::gui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    gui_settings();
    gui_terrain_gen_controls();
    gui_terrain_visualization();
}

// Various Settings
void PerlinNoise::gui_settings() {
    ImGui::Begin("Scene Settings");
    const char* visual_text[3] = { "Planes", "Lines", "Points" };
    ImGui::Combo("Visualization", &visualization_primitive, visual_text, 3);

    ImGui::DragFloat3("Light Position", &light_position[0], 0.01f);
    ImGui::Separator();

    ImGui::DragFloat("Cam Move Speed", &main_camera.get_move_speed(), 0.01f);
    ImGui::DragFloat("Cam Turn Speed", &main_camera.get_turn_speed(), 0.01f);
    ImGui::DragFloat("Cam Turn Sensitivity", &main_camera.get_sensitivity(), 0.001f);
    ImGui::Checkbox("Inverse Turn", &main_camera.get_inverse());
    ImGui::Checkbox("Flight Mode", &main_camera.get_flight());

    ImGui::End();
}

// Terrain Generation Controls
void PerlinNoise::gui_terrain_gen_controls() {
    ImGui::Begin("Terrain Generation Controls");
    if (ImGui::Checkbox("Use DLA Terrain", &useDLA)) {
        RegenerateNoise();
    }

    if (useDLA) {

    }
    else {
        // Existing Perlin noise controls
        if (ImGui::SliderInt("Octave Count", &octaveCount, 1, 8) ||
            ImGui::SliderFloat("Height Multiplier", &heightMultiplier, 0.1f, 5.0f, "%.2f") ||
            ImGui::SliderFloat("Gradient Factor", &gradientFactor, 0.0f, 30.0f, "%.2f") ||
            ImGui::SliderFloat("Persistence", &persistence, 0.1f, 1.0f, "%.2f"))
        {
            RegenerateNoise();
        }
    }

    if (ImGui::Button("Regenerate Terrain")) {
        if (!useDLA) {
            InitializePermutationVector();
        }
        RegenerateNoise();
    }
    ImGui::End();
}

std::vector<float> flatten2DVector(const std::vector<std::vector<float>>& vec2D) {
    std::vector<float> vec1D;
    for (const auto& row : vec2D) {
        for (float value : row) {
            vec1D.push_back(value);
        }
    }
    return vec1D;
}

// Visualization
void PerlinNoise::gui_terrain_visualization() {
    ImGui::Begin("Terrain Visualization");
    ImVec2 previewSize(static_cast<float>(500), static_cast<float>(500));
    ImGui::Text("Perlin Noise Preview");

    static GLuint terrainTextureID = 0;
    if (terrainTextureID == 0) {
        glGenTextures(1, &terrainTextureID);
        glBindTexture(GL_TEXTURE_2D, terrainTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    std::vector<unsigned char> terrainData(outputWidth * outputDepth * 4); // x4 to accommodate RGBA
    const std::vector<float>& sourceData = perlinNoise;
    for (int i = 0; i < outputWidth * outputDepth; ++i) {
        unsigned char value = static_cast<unsigned char>(sourceData[i] * 255.0f);
        terrainData[i * 4 + 0] = value; // R
        terrainData[i * 4 + 1] = value; // G
        terrainData[i * 4 + 2] = value; // B
        terrainData[i * 4 + 3] = 255;   // A (fully opaque)
    }

    glBindTexture(GL_TEXTURE_2D, terrainTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outputWidth, outputDepth, 0, GL_RGBA, GL_UNSIGNED_BYTE, terrainData.data());

    ImGui::Image((void*)(intptr_t)terrainTextureID, previewSize);
    ImGui::End();

    ImGui::Begin("DLA Terrain Preview");

    if (ImGui::Button("Generate Next Stage")) {
        GenerateDLATerrain(1);
    }

    // crisp DLA image
    ImVec2 DLA_preview(500, 500);
    static GLuint terrainTextureID_2 = 1;
    if (terrainTextureID_2 == 1) {
        glGenTextures(1, &terrainTextureID_2);
        glBindTexture(GL_TEXTURE_2D, terrainTextureID_2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    std::vector<float> flat_dlaData = flatten2DVector(dlaData);
    std::vector<unsigned char> terrainData_2(flat_dlaData.size() * 4); // x4 to accommodate RGBA

    for (int i = 0; i < flat_dlaData.size(); ++i) {
        unsigned char value = static_cast<unsigned char>(flat_dlaData[i] * 255.0f);
        terrainData_2[i * 4 + 0] = value; // R
        terrainData_2[i * 4 + 1] = value; // G
        terrainData_2[i * 4 + 2] = value; // B
        terrainData_2[i * 4 + 3] = 255;   // A (fully opaque)
    }

    glBindTexture(GL_TEXTURE_2D, terrainTextureID_2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(dlaData[0].size()), static_cast<GLsizei>(dlaData.size()), 0, GL_RGBA, GL_UNSIGNED_BYTE, terrainData_2.data());

    ImGui::Image((void*)(intptr_t)terrainTextureID_2, DLA_preview);

    // blurry DLA image
    ImVec2 blurry_DLA_preview(500, 500);
    static GLuint terrainTextureID_3 = 2;
    if (terrainTextureID_3 == 2) {
        glGenTextures(1, &terrainTextureID_3);
        glBindTexture(GL_TEXTURE_2D, terrainTextureID_3);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    std::vector<float> flat_blurry_dlaData = flatten2DVector(blurry_dlaData);
    std::vector<unsigned char> terrainData_3(flat_blurry_dlaData.size() * 4); // x4 to accommodate RGBA

    float max_value = *std::max_element(flat_blurry_dlaData.begin(), flat_blurry_dlaData.end());
    if (max_value > 0.f) {
        for (auto& value : flat_blurry_dlaData) {
            value /= max_value;
        }
    }

    for (int i = 0; i < flat_blurry_dlaData.size(); ++i) {
        unsigned char value = static_cast<unsigned char>(std::min(flat_blurry_dlaData[i] * 255.0f, 255.0f));
        terrainData_3[i * 4 + 0] = value; // R
        terrainData_3[i * 4 + 1] = value; // G
        terrainData_3[i * 4 + 2] = value; // B
        terrainData_3[i * 4 + 3] = 255;   // A (fully opaque)
    }

    glBindTexture(GL_TEXTURE_2D, terrainTextureID_3);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(blurry_dlaData[0].size()), static_cast<GLsizei>(blurry_dlaData.size()), 0, GL_RGBA, GL_UNSIGNED_BYTE, terrainData_3.data());

    ImGui::Image((void*)(intptr_t)terrainTextureID_3, blurry_DLA_preview);
    ImGui::End();
}