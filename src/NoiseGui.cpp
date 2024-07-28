
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

    const char* visual_text[3] = { "Perlin", "Perlin + Gradient", "DLA" };
    if (ImGui::Combo("Terrain Generation", &terrain_gen_type, visual_text, 3)) {
        std::cout << terrain_gen_type << std::endl;
    }

    if (terrain_gen_type == 0 || terrain_gen_type == 1) {
        // perlin noise and perlin + gradient
        ImGui::Text("Perlin Noise Settings");
        bool changed = false;
        changed |= ImGui::SliderInt("Octave Count", &octaveCount, 1, 10);
        changed |= ImGui::SliderFloat("Height Multiplier", &heightMultiplier, 0.1f, 5.0f, "%.2f");
        changed |= ImGui::SliderFloat("Gradient Factor", &gradientFactor, 0.0f, 30.0f, "%.2f");
        changed |= ImGui::SliderFloat("Persistence", &persistence, 0.1f, 1.0f, "%.2f");
        if (changed) {
            RegenerateNoise();
        }
        if (ImGui::Button("Regenerate Terrain")) {
            RegenerateNoise();
        }
    }
    else if (terrain_gen_type == 2) {
        // DLA
        ImGui::Text("DLA Settings");
        if (ImGui::Button("Increase DLA Detail")) {
            GenerateDLATerrain(1);
            GenerateTerrainDLA();

            int terrainType = DLA_NOISE;
            glBindBuffer(GL_ARRAY_BUFFER, vbo[terrainType]);
            glBufferData(GL_ARRAY_BUFFER, vertices[terrainType].size() * sizeof(Vertex), vertices[terrainType].data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[terrainType]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[terrainType].size() * sizeof(unsigned int), indices[terrainType].data(), GL_STATIC_DRAW);
        }
        if (ImGui::Button("Regenerate Terrain")) {
            RegenerateNoise();
        }
    }

    ImGui::End();
}

std::vector<float> PerlinNoise::flatten2DVector(const std::vector<std::vector<float>>& vec2D) {
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
    


    if (terrain_gen_type == 0) {
        ImGui::Begin("Perlin Noise Height Maps");
        ImVec2 previewSize(static_cast<float>(400), static_cast<float>(400));

        ImGui::Text("Alternative Perlin Noise Height Map");
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


        ImVec2 previewSize_alt(static_cast<float>(400), static_cast<float>(400));

        ImGui::Text("Alternative Perlin Noise Height Map");
        static GLuint terrainTextureID_1 = 1;
        if (terrainTextureID_1 == 1) {
            glGenTextures(1, &terrainTextureID_1);
            glBindTexture(GL_TEXTURE_2D, terrainTextureID_1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        std::vector<unsigned char> terrainData_1(outputWidth * outputDepth * 4); // x4 to accommodate RGBA
        const std::vector<float>& sourceData_1 = altPerlinNoise;
        for (int i = 0; i < outputWidth * outputDepth; ++i) {
            unsigned char value = static_cast<unsigned char>(sourceData_1[i] * 255.0f);
            terrainData_1[i * 4 + 0] = value; // R
            terrainData_1[i * 4 + 1] = value; // G
            terrainData_1[i * 4 + 2] = value; // B
            terrainData_1[i * 4 + 3] = 255;   // A (fully opaque)
        }
        glBindTexture(GL_TEXTURE_2D, terrainTextureID_1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outputWidth, outputDepth, 0, GL_RGBA, GL_UNSIGNED_BYTE, terrainData_1.data());
        ImGui::Image((void*)(intptr_t)terrainTextureID_1, previewSize_alt);
        ImGui::End();
    }











    if (terrain_gen_type == 2) {
        ImGui::Begin("DLA Height Maps");

        // crisp DLA image
        ImVec2 DLA_preview(400, 400);
        ImGui::Text("Crisp DLA Height Map");
        static GLuint terrainTextureID_2 = 2;
        if (terrainTextureID_2 == 2) {
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
        ImVec2 blurry_DLA_preview(400, 400);
        ImGui::Text("Smoothed DLA Height Map");
        static GLuint terrainTextureID_3 = 3;
        if (terrainTextureID_3 == 3) {
            glGenTextures(1, &terrainTextureID_3);
            glBindTexture(GL_TEXTURE_2D, terrainTextureID_3);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        std::vector<unsigned char> terrainData_3(flat_blurry_dlaData.size() * 4); // x4 to accommodate RGBA
        float max_value = *std::max_element(flat_blurry_dlaData.begin(), flat_blurry_dlaData.end());
        std::vector<float> image_flat_blurry_dlaData = flat_blurry_dlaData;
        if (max_value > 0.f) {
            for (auto& value : image_flat_blurry_dlaData) {
                value /= max_value;
            }
        }

        for (int i = 0; i < image_flat_blurry_dlaData.size(); ++i) {
            unsigned char value = static_cast<unsigned char>(std::min(image_flat_blurry_dlaData[i] * 255.0f, 255.0f));
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
    














    if (terrain_gen_type == 1) {
        ImGui::Begin("Perlin Noise + Gradient Height Maps");
        ImVec2 previewSize_2(static_cast<float>(400), static_cast<float>(400));

        ImGui::Text("Perlin Noise + Gradient Height Map");
        static GLuint terrainTextureID_4 = 4;
        if (terrainTextureID_4 == 4) {
            glGenTextures(1, &terrainTextureID_4);
            glBindTexture(GL_TEXTURE_2D, terrainTextureID_4);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        std::vector<unsigned char> terrainData_4(outputWidth * outputDepth * 4); // x4 to accommodate RGBA
        const std::vector<float>& sourceData_4 = perlinNoiseWithGradient;
        for (int i = 0; i < outputWidth * outputDepth; ++i) {
            unsigned char value = static_cast<unsigned char>(sourceData_4[i] * 255.0f);
            terrainData_4[i * 4 + 0] = value; // R
            terrainData_4[i * 4 + 1] = value; // G
            terrainData_4[i * 4 + 2] = value; // B
            terrainData_4[i * 4 + 3] = 255;   // A (fully opaque)
        }
        glBindTexture(GL_TEXTURE_2D, terrainTextureID_4);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outputWidth, outputDepth, 0, GL_RGBA, GL_UNSIGNED_BYTE, terrainData_4.data());
        ImGui::Image((void*)(intptr_t)terrainTextureID_4, previewSize_2);


        ImVec2 previewSize_2_alt(static_cast<float>(400), static_cast<float>(400));

        ImGui::Text("Alternative Perlin Noise + Gradient Height Map");
        static GLuint terrainTextureID_5 = 5;
        if (terrainTextureID_5 == 5) {
            glGenTextures(1, &terrainTextureID_5);
            glBindTexture(GL_TEXTURE_2D, terrainTextureID_5);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        std::vector<unsigned char> terrainData_5(outputWidth * outputDepth * 4); // x4 to accommodate RGBA
        const std::vector<float>& sourceData_5 = altPerlinNoiseWithGradient;
        for (int i = 0; i < outputWidth * outputDepth; ++i) {
            unsigned char value = static_cast<unsigned char>(sourceData_5[i] * 255.0f);
            terrainData_5[i * 4 + 0] = value; // R
            terrainData_5[i * 4 + 1] = value; // G
            terrainData_5[i * 4 + 2] = value; // B
            terrainData_5[i * 4 + 3] = 255;   // A (fully opaque)
        }
        glBindTexture(GL_TEXTURE_2D, terrainTextureID_5);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outputWidth, outputDepth, 0, GL_RGBA, GL_UNSIGNED_BYTE, terrainData_5.data());
        ImGui::Image((void*)(intptr_t)terrainTextureID_5, previewSize_2_alt);
        ImGui::End();
    }
}