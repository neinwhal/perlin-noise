#include "glhelper.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "PerlinNoise.hpp"

void PerlinNoise::load()
{
    add_shader_programs("terrain", "shaders/terrain.vert", "shaders/terrain.frag");
}


void PerlinNoise::init()
{
    GeneratePerlinNoise();
    GeneratePerlinNoiseWithGradient();

    // Generate terrain for both planes
    GeneratePerlinNoiseTerrain(perlinNoise, vertices[0], indices[0]);
    GeneratePerlinNoiseTerrain(perlinNoiseWithGradient, vertices[1], indices[1]);


    perlinNoise.reserve(outputWidth * outputDepth);
    perlinNoiseWithGradient.reserve(outputWidth * outputDepth);

    // Set up VAOs, VBOs, and EBOs for both planes
    glGenVertexArrays(2, vao);
    glGenBuffers(2, vbo);
    glGenBuffers(2, ebo);

    for (int i = 0; i < 2; ++i)
    {
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


void PerlinNoise::update(double dt)
{
    elapsedTime += dt;
    glViewport(0, 0, GLHelper::width, GLHelper::height);
    float aspectRatio = (float)GLHelper::width / GLHelper::height;
    camera.setAspectRatio(aspectRatio);

    // handle movement controls
    control(dt);

    // ImGui Stuff
    gui();

    // Call draw function to draw the scene
	draw();
}

void PerlinNoise::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    shdrpgms["terrain"].use();

    // Set up camera
    camera.setTarget(camera.getPosition() + GLHelper::cameraFront);
    camera.setFOV(GLHelper::fov);

    // Set up model, view, projection matrices
    glm::mat4 projection = camera.getProjectionMatrix();
    glm::mat4 view = camera.getViewMatrix();

    // Set uniforms
    GLuint uniform_uLightPos = glGetUniformLocation(shdrpgms["terrain"].get_handle(), "uLightPos");
    glUniform3f(uniform_uLightPos, 5.0f, 5.0f, 5.0f);
    GLuint uniform_uViewPos = glGetUniformLocation(shdrpgms["terrain"].get_handle(), "uViewPos");
    glUniform3f(uniform_uViewPos, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

    // Draw terrain
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;
    GLuint uniform_mvpMatrix = glGetUniformLocation(shdrpgms["terrain"].get_handle(), "mvpMatrix");
    glUniformMatrix4fv(uniform_mvpMatrix, 1, GL_FALSE, glm::value_ptr(mvp));

    if (useDLA) {
        glBindVertexArray(vao[0]);
        glDrawElements(GL_TRIANGLES, indices[0].size(), GL_UNSIGNED_INT, 0);
    }
    else {
        // Draw both Perlin noise terrains
        for (int i = 0; i < 2; ++i) {
            model = glm::mat4(1.0f);
            if (i == 1) {
                model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
            }
            mvp = projection * view * model;
            glUniformMatrix4fv(uniform_mvpMatrix, 1, GL_FALSE, glm::value_ptr(mvp));

            glBindVertexArray(vao[i]);
            glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_INT, 0);
        }
    }

    glBindVertexArray(0);
    shdrpgms["terrain"].unuse();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void PerlinNoise::cleanup()
{
    glDeleteVertexArrays(2, vao);
    glDeleteBuffers(2, vbo);
    glDeleteBuffers(2, ebo);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void PerlinNoise::GeneratePerlinNoiseTerrain(const std::vector<float>& noiseData, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices)
{
    outVertices.clear();
    outIndices.clear();

    float minHeight = *std::min_element(noiseData.begin(), noiseData.end());
    float maxHeight = *std::max_element(noiseData.begin(), noiseData.end());

    // Height exaggeration factor
    float heightExaggeration = 1.5f;

    // Adjust water level and color palettes
    float waterLevel = minHeight + (maxHeight - minHeight) * 0.35f;

    glm::vec3 deepWaterColor(0.0f, 0.05f, 0.3f);
    glm::vec3 shallowWaterColor(0.0f, 0.4f, 0.8f);
    glm::vec3 sandColor(0.76f, 0.7f, 0.5f);
    glm::vec3 grassColor(0.0f, 0.5f, 0.0f);
    glm::vec3 forestColor(0.0f, 0.3f, 0.0f);
    glm::vec3 rockColor(0.5f, 0.5f, 0.5f);
    glm::vec3 snowColor(1.0f, 1.0f, 1.0f);

    // Noise functions
    auto noise = [](float x, float y) {
        return glm::fract(sin(x * 12.9898f + y * 78.233f) * 43758.5453f);
        };

    auto turbulence = [&](float x, float y, int octaves) {
        float value = 0.0f;
        float scale = 1.0f;
        for (int i = 0; i < octaves; i++) {
            value += noise(x * scale, y * scale) / scale;
            scale *= 2.0f;
        }
        return value;
        };

    for (int z = 0; z < outputDepth; z++)
    {
        for (int x = 0; x < outputWidth; x++)
        {
            float originalHeight = noiseData[z * outputWidth + x];

            // Exaggerate the height
            float exaggeratedHeight = std::sqrt((originalHeight - minHeight) / (maxHeight - minHeight));
            float y = minHeight + exaggeratedHeight * (maxHeight - minHeight);

            glm::vec3 position(x / static_cast<float>(outputWidth - 1) - 0.5f, y * heightMultiplier, z / static_cast<float>(outputDepth - 1) - 0.5f);
            float normalizedHeight = exaggeratedHeight;  

            // Calculate slope
            float slope = 0.0f;
            if (x > 0 && x < outputWidth - 1 && z > 0 && z < outputDepth - 1) {
                glm::vec3 normal = glm::normalize(glm::cross(
                    glm::vec3(1, noiseData[z * outputWidth + (x + 1)] - originalHeight, 0),
                    glm::vec3(0, noiseData[(z + 1) * outputWidth + x] - originalHeight, 1)
                ));
                slope = 1.0f - normal.y; // Steepness
            }

            glm::vec3 color;
            float vegetationDensity = 0.0f;

            // Adjust thresholds for more visible variety
            if (normalizedHeight < 0.35f)
            {
                float waterDepth = (0.35f - normalizedHeight) / 0.35f;
                color = glm::mix(shallowWaterColor, deepWaterColor, waterDepth);
                position.y = waterLevel * heightMultiplier;
            }
            else if (normalizedHeight < 0.4f)
            {
                float t = (normalizedHeight - 0.35f) / 0.05f;
                color = glm::mix(sandColor, grassColor, t);
                vegetationDensity = t * 0.3f;
            }
            else if (normalizedHeight < 0.7f)
            {
                float t = (normalizedHeight - 0.4f) / 0.3f;
                color = glm::mix(grassColor, forestColor, t);
                vegetationDensity = 0.3f + t * 0.4f;
            }
            else if (normalizedHeight < 0.85f)
            {
                float t = (normalizedHeight - 0.7f) / 0.15f;
                color = glm::mix(forestColor, rockColor, t);
                vegetationDensity = (1.0f - t) * 0.4f;
            }
            else
            {
                float t = (normalizedHeight - 0.85f) / 0.15f;
                color = glm::mix(rockColor, snowColor, t);
                vegetationDensity = 0.0f;
            }

            // Add noise to color
            float colorNoise = (noise(x * 0.1f, z * 0.1f) * 2.0f - 1.0f) * 0.1f;
            color += glm::vec3(colorNoise);

            // Add snow based on elevation and slope
            if (normalizedHeight > 0.8f) {
                float snowAmount = glm::smoothstep(0.8f, 1.0f, normalizedHeight) * (1.0f - slope);
                color = glm::mix(color, snowColor, snowAmount);
            }

            // Add vegetation noise
            float vegetationNoise = turbulence(x * 0.05f, z * 0.05f, 4) * vegetationDensity;
            color = glm::mix(color, forestColor, vegetationNoise * 0.3f);

            // Add subtle variations to terrain color
            float terrainVariation = noise(x * 0.02f, z * 0.02f) * 0.1f - 0.05f;
            color += glm::vec3(terrainVariation);

            color = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));

            outVertices.push_back({ position, color });
        }
    }

    // Generate indices (same as before)
    for (int z = 0; z < outputDepth - 1; z++)
    {
        for (int x = 0; x < outputWidth - 1; x++)
        {
            int topLeft = z * outputWidth + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * outputWidth + x;
            int bottomRight = bottomLeft + 1;

            outIndices.push_back(topLeft);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(topRight);

            outIndices.push_back(topRight);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(bottomRight);
        }
    }
}

void  PerlinNoise::InitializePermutationVector() {
    p.resize(512);
    std::iota(p.begin(), p.begin() + 256, 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(p.begin(), p.begin() + 256, g);
    std::copy(p.begin(), p.begin() + 256, p.begin() + 256);
}

/**
 * @brief Generates Perlin noise for a given point (x, y, z) in 3D space.
 *
 * This function computes the Perlin noise value for a given 3D point by interpolating
 * between gradients at surrounding grid points. It uses a combination of gradient
 * vectors, fade curves, and linear interpolation to achieve smooth transitions.
 *
 * @param x The x-coordinate of the input point.
 * @param y The y-coordinate of the input point.
 * @param z The z-coordinate of the input point.
 * @return A float representing the Perlin noise value at the given point.
 *
 * Example:
 * ```
 * PerlinNoise perlin;
 * float noiseValue = perlin.Noise(1.5, 2.5, 3.5);
 * // This will generate the Perlin noise value at the point (1.5, 2.5, 3.5).
 * ```
 */
float PerlinNoise::Noise(float x, float y, float z) {
    // Determine the grid cell coordinates containing the point (x, y, z)
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    // Get the relative position of the point within the grid cell
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    // Compute fade curves for x, y, z
    float u = Fade(x);
    float v = Fade(y);
    float w = Fade(z);

    // Hash coordinates of the 8 cube corners
    int A = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    // Interpolate the results from the 8 corners of the cube
    // The interpolation is performed in three dimensions:
    return Lerp(w,
        Lerp(v,
            Lerp(u,
                Grad(p[AA], x, y, z),
                Grad(p[BA], x - 1, y, z)
            ),
            Lerp(u,
                Grad(p[AB], x, y - 1, z),
                Grad(p[BB], x - 1, y - 1, z)
            )
        ),
        Lerp(v,
            Lerp(u,
                Grad(p[AA + 1], x, y, z - 1),
                Grad(p[BA + 1], x - 1, y, z - 1)
            ),
            Lerp(u,
                Grad(p[AB + 1], x, y - 1, z - 1),
                Grad(p[BB + 1], x - 1, y - 1, z - 1)
            )
        )
    );
}

/**
 * @brief Generates Perlin noise with multiple octaves to add finer details.
 *
 * This function combines multiple layers (octaves) of Perlin noise at different
 * frequencies and amplitudes. Each octave adds finer details to the noise.
 *
 * @param x The x-coordinate of the input point.
 * @param y The y-coordinate of the input point.
 * @param z The z-coordinate of the input point.
 * @param octaves The number of layers (octaves) of noise to combine.
 * @param persistence Controls the decrease in amplitude of each subsequent octave.
 * @return A float representing the Perlin noise value at the given point.
 *
 * Example:
 * ```
 * PerlinNoise perlin;
 * float noiseValue = perlin.OctaveNoise(0.5, 0.5, 0.5, 4, 0.5);
 * // This will generate Perlin noise at point (0.5, 0.5, 0.5) using 4 octaves and a persistence of 0.5.
 * ```
 *
 * Visual Example:
 * Consider generating noise with 3 octaves (octaves = 3) and persistence = 0.5:
 * 1. Octave 1: frequency = 1, amplitude = 1
 * 2. Octave 2: frequency = 2, amplitude = 0.5
 * 3. Octave 3: frequency = 4, amplitude = 0.25
 *
 * For a point (x, y, z):
 * - Calculate the noise value at each octave with the corresponding frequency and amplitude.
 * - Sum the weighted noise values.
 * - Normalize the result to ensure it stays within the range [0, 1].
 */
float PerlinNoise::OctaveNoise(float x, float y, float z, int octaves, float persistence) {
    float total     = 0;   // Total accumulated noise value
    float frequency = 1;   // Initial frequency
    float amplitude = 1;   // Initial amplitude
    float maxValue  = 0;   // Accumulates the total of all amplitudes for normalization

    // Loop through each octave
    for (int i = 0; i < octaves; i++) {
        // Calculate noise for the current octave and add it to the total
        total += Noise(x * frequency, y * frequency, z * frequency) * amplitude;

        // Accumulate the current amplitude to maxValue
        maxValue += amplitude;

        // Adjust amplitude and frequency for the next octave
        amplitude *= persistence;
        frequency *= 2;
    }

    // Normalize the result to ensure it stays within the range [0, 1]
    return total / maxValue;
}

/**
 * @brief Generates a 2D array of Perlin noise values.
 *
 * This function generates a 2D array of Perlin noise values using multiple
 * octaves for added detail. The noise values are clamped to the range [0, 1].
 *
 *
 * Visual Example:
 * Imagine a 2D grid of size 4x4. Each cell in the grid will get a Perlin noise value.
 * Let's assume `octaveCount = 2` and `persistence = 0.5`.
 *
 * 1. Normalize coordinates to [-0.5, 0.5]:
 *    - For x = 0: nx = 0 / 4 - 0.5 = -0.5
 *    - For y = 0: ny = 0 / 4 - 0.5 = -0.5
 * 2. Calculate OctaveNoise for each point:
 *    - Octave 1: Noise(-0.5, -0.5, 0.5) * 1
 *    - Octave 2: Noise(-1.0, -1.0, 1.0) * 0.5
 *    - Sum and normalize the result.
 *
 * @details The perlinNoise array will be populated with values for a visual representation, such as:
 * [ 0.43, 0.55, 0.62, 0.50,
 *   0.48, 0.60, 0.70, 0.58,
 *   0.52, 0.63, 0.73, 0.62,
 *   0.50, 0.60, 0.68, 0.57 ]
 */
void PerlinNoise::GeneratePerlinNoise() {
    // Loop through each pixel in the output grid
    for (int y = 0; y < outputDepth; y++) {
        for (int x = 0; x < outputWidth; x++) {
            // Normalize the coordinates to the range [-0.5, 0.5]
            float nx = (float)x / outputWidth - 0.5f;
            float ny = (float)y / outputDepth - 0.5f;

            // Generate the Perlin noise value at the current coordinates using octave noise
            float value = OctaveNoise(nx, ny, 0.5f, octaveCount, persistence);

            // Clamp the noise value to the range [0, 1] and store it in the output array
            value = std::max(0.0f, std::min(1.0f, (value + 1.0f) * 0.5f));
            perlinNoise[y * outputWidth + x] = value;
        }
    }
}

glm::vec2 PerlinNoise::CalculateGradient(int x, int z, int width, int depth, const std::vector<float>& noise)
{
    float epsilon = 1.0f;
    float heightCenter = noise[z * width + x];
    float heightRight = (x < width - 1) ? noise[z * width + (x + 1)] : heightCenter;
    float heightUp = (z < depth - 1) ? noise[(z + 1) * width + x] : heightCenter;

    return glm::vec2((heightRight - heightCenter) / epsilon, (heightUp - heightCenter) / epsilon);
}

// New method to generate DLA terrain
void PerlinNoise::GenerateDLATerrain() {
    dlaGenerator.SetOutputSize(outputWidth, outputDepth);
    dlaTerrain = dlaGenerator.GenerateTerrain();

    if (dlaTerrain.size() != outputWidth * outputDepth) {
        ResizeTerrain(dlaTerrain, outputWidth, outputDepth);
    }
}

void PerlinNoise::GenerateDLATerrainMesh(std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices)
{
    outVertices.clear();
    outIndices.clear();

    float minHeight = *std::min_element(dlaTerrain.begin(), dlaTerrain.end());
    float maxHeight = *std::max_element(dlaTerrain.begin(), dlaTerrain.end());
    float heightRange = maxHeight - minHeight;

    if (heightRange < 0.001f) {
        heightRange = 1.0f;
    }

    for (int z = 0; z < outputDepth; z++) {
        for (int x = 0; x < outputWidth; x++) {
            float height = dlaTerrain[z * outputWidth + x];
            float normalizedHeight = (height - minHeight) / heightRange;

            // Apply height adjustments
            normalizedHeight = std::pow(normalizedHeight, dlaHeightPower);
            normalizedHeight = normalizedHeight * dlaHeightScale + dlaHeightOffset;
            normalizedHeight = std::max(0.0f, std::min(1.0f, normalizedHeight));  // Clamp to [0, 1]

            glm::vec3 position(
                x / static_cast<float>(outputWidth - 1) - 0.5f,
                normalizedHeight * heightMultiplier,
                z / static_cast<float>(outputDepth - 1) - 0.5f
            );

            // Color calculation (you can adjust this as needed)
            glm::vec3 color;
            if (normalizedHeight < 0.2f) {
                color = glm::vec3(0.0f, 0.0f, 0.5f); // Deep blue for water
            }
            else if (normalizedHeight < 0.4f) {
                color = glm::vec3(0.76f, 0.7f, 0.5f); // Sand color for beaches
            }
            else if (normalizedHeight < 0.6f) {
                color = glm::vec3(0.0f, 0.5f, 0.0f); // Green for lowlands
            }
            else if (normalizedHeight < 0.8f) {
                color = glm::vec3(0.5f, 0.25f, 0.0f); // Brown for highlands
            }
            else {
                color = glm::vec3(1.0f, 1.0f, 1.0f); // White for peaks
            }

            outVertices.push_back({ position, color });
        }
    }
    // Generate indices (same as before)
    for (int z = 0; z < outputDepth - 1; z++) {
        for (int x = 0; x < outputWidth - 1; x++) {
            int topLeft = z * outputWidth + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * outputWidth + x;
            int bottomRight = bottomLeft + 1;

            outIndices.push_back(topLeft);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(topRight);
            outIndices.push_back(topRight);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(bottomRight);
        }
    }
}

void PerlinNoise::ResizeTerrain(std::vector<float>& terrain, int width, int depth)
{
    std::vector<float> resizedTerrain(width * depth);
    int originalWidth = static_cast<int>(std::sqrt(terrain.size()));
    int originalDepth = originalWidth;

    for (int z = 0; z < depth; z++) {
        for (int x = 0; x < width; x++) {
            float srcX = x * (originalWidth - 1) / static_cast<float>(width - 1);
            float srcZ = z * (originalDepth - 1) / static_cast<float>(depth - 1);
            int x0 = static_cast<int>(srcX);
            int z0 = static_cast<int>(srcZ);
            int x1 = std::min(x0 + 1, originalWidth - 1);
            int z1 = std::min(z0 + 1, originalDepth - 1);

            float fx = srcX - x0;
            float fz = srcZ - z0;

            float h00 = terrain[z0 * originalWidth + x0];
            float h10 = terrain[z0 * originalWidth + x1];
            float h01 = terrain[z1 * originalWidth + x0];
            float h11 = terrain[z1 * originalWidth + x1];

            float h0 = h00 * (1 - fx) + h10 * fx;
            float h1 = h01 * (1 - fx) + h11 * fx;

            resizedTerrain[z * width + x] = h0 * (1 - fz) + h1 * fz;
        }
    }

    terrain = std::move(resizedTerrain);
}

void PerlinNoise::GeneratePerlinNoiseWithGradient() {
    const int layerCount = 4; // Number of layers to generate
    const float k = gradientFactor; // Gradient influence parameter

    std::vector<std::vector<float>> layers(layerCount, std::vector<float>(outputWidth * outputDepth));
    std::vector<std::vector<glm::vec2>> gradients(layerCount, std::vector<glm::vec2>(outputWidth * outputDepth));

    // 1. Generate base noise for each layer
    for (int layer = 0; layer < layerCount; ++layer) {
        for (int y = 0; y < outputDepth; ++y) {
            for (int x = 0; x < outputWidth; ++x) {
                float nx = (float)x / outputWidth - 0.5f;
                float ny = (float)y / outputDepth - 0.5f;
                float value = OctaveNoise(nx, ny, 0.5f, octaveCount, persistence);
                value = std::max(0.0f, std::min(1.0f, (value + 1.0f) * 0.5f));
                layers[layer][y * outputWidth + x] = value;
            }
        }
    }

    // 2. Calculate gradients for each layer
    for (int layer = 0; layer < layerCount; ++layer) {
        for (int y = 0; y < outputDepth; ++y) {
            for (int x = 0; x < outputWidth; ++x) {
                gradients[layer][y * outputWidth + x] = CalculateGradient(x, y, outputWidth, outputDepth, layers[layer]);
            }
        }
    }

    // 3. Apply gradient influence and accumulate layers
    std::fill(perlinNoiseWithGradient.begin(), perlinNoiseWithGradient.end(), 0.0f);
    for (int layer = 0; layer < layerCount; ++layer) {
        for (int y = 0; y < outputDepth; ++y) {
            for (int x = 0; x < outputWidth; ++x) {
                int index = y * outputWidth + x;
                glm::vec2 gradient = gradients[layer][index];
                float gradientMagnitude = glm::length(gradient);

                // Calculate gradient influence using the formula
                float gradientInfluence = 1.0f / (1.0f + k * gradientMagnitude);

                // Apply gradient influence and accumulate
                float adjustedValue = layers[layer][index] * gradientInfluence;
                perlinNoiseWithGradient[index] += adjustedValue;
            }
        }
    }

    // 4. Normalize final values
    float minValue = *std::min_element(perlinNoiseWithGradient.begin(), perlinNoiseWithGradient.end());
    float maxValue = *std::max_element(perlinNoiseWithGradient.begin(), perlinNoiseWithGradient.end());
    for (float& value : perlinNoiseWithGradient) {
        value = (value - minValue) / (maxValue - minValue);
    }
}

void PerlinNoise::RegeneratePerlinNoise() {
    if (useDLA) {
        dlaGenerator.SetOutputSize(outputWidth, outputDepth);
        dlaGenerator.SetInitialPoints(dlaInitialPoints);
        dlaGenerator.SetErosionIterations(dlaErosionIterations);
        dlaGenerator.SetErosionStrength(dlaErosionStrength);
        dlaGenerator.SetSmoothingPasses(dlaSmoothingPasses);
        dlaTerrain = dlaGenerator.GenerateTerrain();
        GenerateDLATerrainMesh(vertices[0], indices[0]);
    }
    else {
        GeneratePerlinNoise();
        GeneratePerlinNoiseWithGradient();
        GeneratePerlinNoiseTerrain(perlinNoise, vertices[0], indices[0]);
        GeneratePerlinNoiseTerrain(perlinNoiseWithGradient, vertices[1], indices[1]);
    }

    // Update GPU buffers
    for (int i = 0; i < (useDLA ? 1 : 2); ++i) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
        glBufferData(GL_ARRAY_BUFFER, vertices[i].size() * sizeof(Vertex), vertices[i].data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(unsigned int), indices[i].data(), GL_STATIC_DRAW);
    }
}


