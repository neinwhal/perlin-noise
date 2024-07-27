#include "Noise.hpp"


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