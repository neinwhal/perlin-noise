
#include "Noise.hpp"

glm::vec2 PerlinNoise::CalculateGradient(int x, int z, int width, int depth, const std::vector<float>& noise) {
    float epsilon = 1.0f;
    float heightCenter = noise[z * width + x];
    float heightRight = (x < width - 1) ? noise[z * width + (x + 1)] : heightCenter;
    float heightUp = (z < depth - 1) ? noise[(z + 1) * width + x] : heightCenter;

    return glm::vec2((heightRight - heightCenter) / epsilon, (heightUp - heightCenter) / epsilon);
}

void PerlinNoise::GenerateAltPerlinNoiseWithGradient() {
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
    std::fill(altPerlinNoiseWithGradient.begin(), altPerlinNoiseWithGradient.end(), 0.0f);
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
                altPerlinNoiseWithGradient[index] += adjustedValue;
            }
        }
    }

    // 4. Normalize final values
    float minValue = *std::min_element(altPerlinNoiseWithGradient.begin(), altPerlinNoiseWithGradient.end());
    float maxValue = *std::max_element(altPerlinNoiseWithGradient.begin(), altPerlinNoiseWithGradient.end());
    for (float& value : altPerlinNoiseWithGradient) {
        value = (value - minValue) / (maxValue - minValue);
    }
}

void PerlinNoise::GeneratePerlindNoiseWithGradient()
{
    const int layerCount = 4; // Number of layers to generate
    const float k = gradientFactor; // Gradient influence parameter
    std::vector<std::vector<float>> layers(layerCount, std::vector<float>(outputWidth * outputDepth));
    std::vector<std::vector<glm::vec2>> gradients(layerCount, std::vector<glm::vec2>(outputWidth * outputDepth));

    // 1. Generate base noise for each layer using Proper Perlin Noise
    for (int layer = 0; layer < layerCount; ++layer) {
        for (int y = 0; y < outputDepth; ++y) {
            for (int x = 0; x < outputWidth; ++x) {
                float nx = static_cast<float>(x) / outputWidth;
                float ny = static_cast<float>(y) / outputDepth;
                float value = GenerateNoise(nx * 4.0f, ny * 4.0f, outputWidth, outputDepth, octaveCount);
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
    perlinNoiseWithGradient.resize(outputWidth * outputDepth);
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