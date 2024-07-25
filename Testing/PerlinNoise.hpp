#pragma once
#include "glapp.h"
#include "./Camera.h"
#include "./Primitives.h"
#include "./IntersectionTest.h"
#include <vector>
#include <random>
#include <algorithm>
#include <numeric> 
#include <thread>

#pragma once
#include <vector>
#include <random>
#include <algorithm>
#include <glm/glm.hpp>

#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <iostream>

class ImprovedDLATerrainGenerator {
public:
    ImprovedDLATerrainGenerator(int seed = 134137)
        : m_rng(seed), m_seed(seed), m_initialPoints(5), m_erosionIterations(50000),
        m_erosionStrength(0.1f), m_smoothingPasses(2) {}

    void SetOutputSize(int width, int height) {
        m_outputWidth = width;
        m_outputHeight = height;
    }

    void SetInitialPoints(int points) { m_initialPoints = points; }
    void SetErosionIterations(int iterations) { m_erosionIterations = iterations; }
    void SetErosionStrength(float strength) { m_erosionStrength = strength; }
    void SetSmoothingPasses(int passes) { m_smoothingPasses = passes; }

    std::vector<float> GenerateTerrain() {
        std::vector<float> terrain = PerformDLA();
        ApplyErosion(terrain);
        SmoothTerrain(terrain);
        NormalizeTerrain(terrain);
        return terrain;
    }

private:
    std::mt19937 m_rng;
    int m_seed;
    int m_outputWidth = 128;
    int m_outputHeight = 128;
    int m_initialPoints;
    int m_erosionIterations;
    float m_erosionStrength;
    int m_smoothingPasses;

    std::vector<float> PerformDLA() {
        std::vector<float> terrain(m_outputWidth * m_outputHeight, 0.0f);
        std::vector<std::pair<int, int>> points;

        for (int i = 0; i < m_initialPoints; ++i) {
            int x = m_rng() % m_outputWidth;
            int y = m_rng() % m_outputHeight;
            points.emplace_back(x, y);
            terrain[y * m_outputWidth + x] = 1.0f;
        }

        int maxPoints = m_outputWidth * m_outputHeight / 100; 
        while (points.size() < maxPoints) {
            std::pair<int, int> newPoint = GeneratePerimeterPoint();
            WalkPointUntilStick(newPoint, terrain, points);
        }

        return terrain;
    }

    std::pair<int, int> GeneratePerimeterPoint() {
        std::uniform_int_distribution<> distSide(0, 3);
        std::uniform_int_distribution<> distPos(0, m_outputWidth - 1);

        int side = distSide(m_rng);
        switch (side) {
        case 0: return { 0, distPos(m_rng) }; // Left
        case 1: return { m_outputWidth - 1, distPos(m_rng) }; // Right
        case 2: return { distPos(m_rng), 0 }; // Top
        case 3: return { distPos(m_rng), m_outputHeight - 1 }; // Bottom
        }
        return { 0, 0 }; // Should never reach here la hor
    }

    void WalkPointUntilStick(std::pair<int, int>& point, std::vector<float>& terrain, std::vector<std::pair<int, int>>& points) {
        std::uniform_int_distribution<> distDir(-1, 1);

        while (true) {
            // Check if the point is adjacent to an existing point
            if (IsAdjacentToExistingPoint(point, terrain)) {
                terrain[point.first + point.second * m_outputWidth] = 1.0f;
                points.push_back(point);
                break;
            }

            // Move the point
            point.first = std::clamp(point.first + distDir(m_rng), 0, m_outputWidth - 1);
            point.second = std::clamp(point.second + distDir(m_rng), 0, m_outputHeight - 1);
        }
    }

    bool IsAdjacentToExistingPoint(const std::pair<int, int>& point, const std::vector<float>& terrain) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int nx = point.first + dx;
                int ny = point.second + dy;
                if (nx >= 0 && nx < m_outputWidth && ny >= 0 && ny < m_outputHeight) {
                    if (terrain[nx + ny * m_outputWidth] > 0) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void ApplyMultipleBlurPasses(std::vector<float>& terrain) {
        std::vector<int> blurRadii = { 32, 16, 8, 4, 2, 1 };
        std::vector<float> blurWeights = { 0.01f, 0.04f, 0.08f, 0.16f, 0.32f, 0.39f };

        std::vector<float> result(terrain.size(), 0.0f);

        for (size_t i = 0; i < blurRadii.size(); ++i) {
            std::vector<float> blurred = ApplyBoxBlur(terrain, blurRadii[i]);
            for (size_t j = 0; j < result.size(); ++j) {
                result[j] += blurred[j] * blurWeights[i];
            }
        }

        terrain = result;
    }

    std::vector<float> ApplyBoxBlur(const std::vector<float>& input, int radius) {
        std::vector<float> output = input;
        std::vector<float> temp(input.size());

        // Horizontal pass
        for (int y = 0; y < m_outputHeight; ++y) {
            float sum = 0.0f;
            for (int x = 0; x < radius; ++x) {
                sum += input[x + y * m_outputWidth];
            }
            for (int x = 0; x < m_outputWidth; ++x) {
                if (x > radius) sum -= input[(x - radius - 1) + y * m_outputWidth];
                if (x < m_outputWidth - radius) sum += input[(x + radius) + y * m_outputWidth];
                temp[x + y * m_outputWidth] = sum / (2 * radius + 1);
            }
        }

        // Vertical pass
        for (int x = 0; x < m_outputWidth; ++x) {
            float sum = 0.0f;
            for (int y = 0; y < radius; ++y) {
                sum += temp[x + y * m_outputWidth];
            }
            for (int y = 0; y < m_outputHeight; ++y) {
                if (y > radius) sum -= temp[x + (y - radius - 1) * m_outputWidth];
                if (y < m_outputHeight - radius) sum += temp[x + (y + radius) * m_outputWidth];
                output[x + y * m_outputWidth] = sum / (2 * radius + 1);
            }
        }

        return output;
    }

    void AddPerlinNoise(std::vector<float>& terrain) {
        // TODO: Implement Perlin noise generation here
        for (int y = 0; y < m_outputHeight; ++y) {
            for (int x = 0; x < m_outputWidth; ++x) {
                float noise = GenerateSimpleNoise(x, y);
                terrain[x + y * m_outputWidth] += noise * 0.1f; 
            }
        }
    }

    float GenerateSimpleNoise(int x, int y) {
        x = (x << 13) ^ y;
        return (1.0f - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
    }

    void ApplyErosion(std::vector<float>& terrain) {
        for (int i = 0; i < m_erosionIterations; ++i) {
            ErosionDroplet(terrain);
        }
    }

    void ErosionDroplet(std::vector<float>& terrain) {
        std::uniform_int_distribution<> distX(0, m_outputWidth - 1);
        std::uniform_int_distribution<> distY(0, m_outputHeight - 1);
        std::uniform_real_distribution<float> distFloat(0.0f, 1.0f);

        int x = distX(m_rng);
        int y = distY(m_rng);
        float sediment = 0.0f;

        for (int step = 0; step < 100; ++step) {
            int index = x + y * m_outputWidth;
            float height = terrain[index];

            // Find the steepest downhill direction
            int newX = x, newY = y;
            float maxDiff = 0.0f;

            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx < 0 || nx >= m_outputWidth || ny < 0 || ny >= m_outputHeight) continue;

                    float diff = height - terrain[nx + ny * m_outputWidth];
                    if (diff > maxDiff) {
                        maxDiff = diff;
                        newX = nx;
                        newY = ny;
                    }
                }
            }

            if (maxDiff == 0.0f) break; // No downhill path

            // Erode and deposit
            float erosionFactor = 0.1f;
            float depositFactor = 0.1f;

            float erosionAmount = std::min(maxDiff, erosionFactor * (1.0f + sediment));
            terrain[index] -= erosionAmount;
            sediment += erosionAmount;

            float depositAmount = std::min(sediment, depositFactor);
            terrain[newX + newY * m_outputWidth] += depositAmount;
            sediment -= depositAmount;

            x = newX;
            y = newY;
        }
    }

    void SmoothTerrain(std::vector<float>& terrain) {
        std::vector<float> temp(terrain.size());
        for (int pass = 0; pass < m_smoothingPasses; ++pass) {
            for (int y = 1; y < m_outputHeight - 1; ++y) {
                for (int x = 1; x < m_outputWidth - 1; ++x) {
                    float sum = 0.0f;
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            sum += terrain[(y + dy) * m_outputWidth + (x + dx)];
                        }
                    }
                    temp[y * m_outputWidth + x] = sum / 9.0f;
                }
            }
            std::swap(terrain, temp);
        }
    }

    void NormalizeTerrain(std::vector<float>& terrain) {
        float minHeight = *std::min_element(terrain.begin(), terrain.end());
        float maxHeight = *std::max_element(terrain.begin(), terrain.end());
        float range = maxHeight - minHeight;

        for (float& height : terrain) {
            height = (height - minHeight) / range;
        }
    }
};
class PerlinNoise : public Scene_Base
{
public:
    PerlinNoise(Scene_Manager& sceneManager, const std::string& name)
        : _sceneManager(sceneManager), _name(name), elapsedTime(0.0f),
        m_cameraSpeed(2.5f), m_cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), m_cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
        camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, (float)GLHelper::width / (float)GLHelper::height, 0.1f, 100.0f),
        outputWidth(650), outputDepth(650), octaveCount(8), persistence(0.5f), heightMultiplier(0.07f),
        gradientFactor(5.03f), useDLA(false),
        dlaInitialPoints(1),
        dlaErosionIterations(1),
        dlaErosionStrength(0.1f),
        dlaSmoothingPasses(2),
        dlaGenerator(134137),
        dlaHeightScale(1.0f),
        dlaHeightOffset(0.0f),
        dlaHeightPower(1.0f)
    {
        perlinNoise.resize(outputWidth * outputDepth);
        perlinNoiseWithGradient.resize(outputWidth * outputDepth);
        InitializePermutationVector();
    }
    virtual void load() override;
    virtual void init() override;
    virtual void update(double dt) override;
    virtual void draw() override;
    virtual void cleanup() override;

private:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
    };

    // Scene Manager
    std::string _name;
    Scene_Manager& _sceneManager;
    double elapsedTime;

    // Shader
    GLSLShader shdr_pgm;

    // Camera
    Camera camera;
    float m_cameraSpeed;
    glm::vec3 m_cameraFront;
    glm::vec3 m_cameraUp;

    // Perlin Noise Stuff
    int outputWidth, outputDepth;
    std::vector<float> perlinNoise;
    std::vector<float> perlinNoiseWithGradient;
    int octaveCount;
    float persistence;
    float heightMultiplier;
    float gradientFactor;
    std::vector<int> p;

    // Perlin Noise functions
    void InitializePermutationVector();
    float Noise(float x, float y, float z);
    float OctaveNoise(float x, float y, float z, int octaves, float persistence);

    /**
     * @brief Takes input t and creates a smooth curve that eases in and out.
     *
     * This function smooths the input value `t` using a polynomial function. Smoothing
     * helps to avoid hard edges in the noise, thus making it look more natural.
     *
     * @param t The input value to be smoothed, typically in the range [0, 1].
     * @return A float representing the smoothed value.
     *
     * Example:
     * ```
     * float t = 0.5;
     * float smoothed = Fade(t);
     * // smoothed will be 0.5 * 0.5 * 0.5 * (0.5 * (0.5 * 6 - 15) + 10) = 0.5
     * ```
     */
    static inline float Fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    /**
     * @brief Linearly interpolates between values a and b by t.
     *
     * This function performs linear interpolation between two values `a` and `b` based on the
     * parameter `t`. If `t` is 0, the result is `a`. If `t` is 1, the result is `b`. Values
     * between 0 and 1 will result in a mix of `a` and `b`.
     *
     * @param t The interpolation parameter, typically in the range [0, 1].
     * @param a The start value.
     * @param b The end value.
     * @return A float representing the interpolated value.
     *
     * Example:
     * ```
     * float a = 0.0;
     * float b = 1.0;
     * float t = 0.5;
     * float result = Lerp(t, a, b);
     * // result will be 0.5, the midpoint between a and b.
     * ```
     */
    static inline float Lerp(float t, float a, float b) {
        return a + t * (b - a);
    }

    /**
     * @brief Generates a gradient vector based on the hash value.
     *
     * This function generates a gradient vector based on the input hash value. The gradient
     * vectors are used to calculate the dot product between the distance vector and the
     * gradient vector. Bitwise operations determine which direction the gradient should
     * point. The gradients at each grid point influence the noise value at a given point,
     * creating the variation needed for the noise pattern.
     *
     * @param hash The hash value used to determine the gradient direction.
     * @param x The x-coordinate of the distance vector.
     * @param y The y-coordinate of the distance vector.
     * @param z The z-coordinate of the distance vector.
     * @return A float representing the dot product of the distance and gradient vectors.
     *
     * Example:
     * ```
     * int hash = 12;
     * float x = 0.5;
     * float y = 0.5;
     * float z = 0.5;
     * float gradient = Grad(hash, x, y, z);
     * // gradient will be calculated based on the hash value and the coordinates.
     * ```
     */   
     static inline float Grad(int hash, float x, float y, float z) {
        int h = hash & 15;
        float u = h < 8 ? x : y;
        float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    void GeneratePerlinNoise();
    void GeneratePerlinNoiseWithGradient();
    void GeneratePerlinNoiseTerrain(const std::vector<float>& noiseData, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices);
    glm::vec2 CalculateGradient(int x, int z, int width, int depth, const std::vector<float>& noise);
    void GenerateDLATerrain();
	void GenerateDLATerrainMesh(std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices);
	void ResizeTerrain(std::vector<float>& terrain, int width, int depth);

    // Regenerate
    void RegeneratePerlinNoise();



    // DLA Generaor
    bool useDLA;
    int dlaInitialSize;
    int dlaTargetSize;
    ImprovedDLATerrainGenerator dlaGenerator;
    std::vector<float> dlaTerrain;
    int dlaInitialPoints;
    int dlaErosionIterations;
    float dlaErosionStrength;
    int dlaSmoothingPasses;
    float dlaHeightScale;
    float dlaHeightOffset;
    float dlaHeightPower;

    // Buffer Stuff - for perlin noise plane and the perlin noise + gradient plane
    GLuint vao[2]{}, vbo[2]{}, ebo[2]{};
    std::vector<Vertex> vertices[2];
    std::vector<unsigned int> indices[2];
};