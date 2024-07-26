
#include "PerlinNoise.hpp"

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
        RegeneratePerlinNoise();
    }

    if (useDLA) {
        bool dlaParamsChanged = false;
        dlaParamsChanged |= ImGui::SliderInt("Initial Points", &dlaInitialPoints, 1, 20);
        dlaParamsChanged |= ImGui::SliderInt("Erosion Iterations", &dlaErosionIterations, 1000, 100000);
        dlaParamsChanged |= ImGui::SliderFloat("Erosion Strength", &dlaErosionStrength, 0.01f, 1.0f, "%.3f");
        dlaParamsChanged |= ImGui::SliderInt("Smoothing Passes", &dlaSmoothingPasses, 0, 10);

        dlaParamsChanged |= ImGui::SliderFloat("Height Scale", &dlaHeightScale, 0.1f, 2.0f, "%.2f");
        dlaParamsChanged |= ImGui::SliderFloat("Height Offset", &dlaHeightOffset, -0.5f, 0.5f, "%.2f");
        dlaParamsChanged |= ImGui::SliderFloat("Height Power", &dlaHeightPower, 0.1f, 2.0f, "%.2f");
        dlaParamsChanged |= ImGui::SliderFloat("Height Multiplier", &heightMultiplier, 0.1f, 5.0f, "%.2f");

        if (dlaParamsChanged) {
            RegeneratePerlinNoise();
        }
    }
    else {
        // Existing Perlin noise controls
        if (ImGui::SliderInt("Octave Count", &octaveCount, 1, 8) ||
            ImGui::SliderFloat("Height Multiplier", &heightMultiplier, 0.1f, 5.0f, "%.2f") ||
            ImGui::SliderFloat("Gradient Factor", &gradientFactor, 0.0f, 30.0f, "%.2f") ||
            ImGui::SliderFloat("Persistence", &persistence, 0.1f, 1.0f, "%.2f"))
        {
            RegeneratePerlinNoise();
        }
    }

    if (ImGui::Button("Regenerate Terrain")) {
        if (!useDLA) {
            InitializePermutationVector();
        }
        RegeneratePerlinNoise();
    }
    ImGui::End();
}

// Visualization
void PerlinNoise::gui_terrain_visualization() {
    ImGui::Begin("Terrain Visualization");
    ImVec2 previewSize(400, 400);
    ImGui::Text(useDLA ? "DLA Terrain Preview" : "Perlin Noise Preview");

    static GLuint terrainTextureID = 0;
    if (terrainTextureID == 0) {
        glGenTextures(1, &terrainTextureID);
        glBindTexture(GL_TEXTURE_2D, terrainTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    std::vector<unsigned char> terrainData(outputWidth * outputDepth * 4);
    const std::vector<float>& sourceData = useDLA ? dlaTerrain : perlinNoise;
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
}