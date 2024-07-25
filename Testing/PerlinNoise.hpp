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

class DLATerrainGenerator {
public:
    DLATerrainGenerator() : m_initialSize(16), m_targetSize(256) {
        m_rng.seed(std::random_device()());
    }

    void SetInitialSize(int size) { m_initialSize = size; }
    void SetTargetSize(int size) { m_targetSize = size; }

    std::vector<float> GenerateTerrain() {
        std::vector<std::vector<float>> heightMaps;
        heightMaps.push_back(GenerateLowResStartingImage());

        while (static_cast<int>(std::sqrt(heightMaps.back().size())) < m_targetSize) {
            std::vector<float> upscaledHeightMap = UpscaleHeightMap(heightMaps.back());

            // Create a blurry version at the current scale
            std::vector<float> blurryHeightMap = FinalBlur(upscaledHeightMap);

            // Normalize and enhance contrast for blurry version
            NormalizeAndEnhanceContrast(blurryHeightMap);

            // Add details from the crisp version to the blurry version
            AddDetails(blurryHeightMap, upscaledHeightMap);

            // Normalize and enhance contrast for contrast version
            NormalizeAndEnhanceContrast(upscaledHeightMap);

            // Use the combined version as the new heightMap for the next iteration
            heightMaps.push_back(blurryHeightMap);
        }

        // Apply the formula to each height map
        for (auto& heightMap : heightMaps) {
            ApplyFormula(heightMap);
        }

        std::vector<float> finalHeightMap = heightMaps.back();
        NormalizeAndEnhanceContrast(finalHeightMap);
        // Debug output
        std::cout << "DLA Terrain Generation:" << std::endl;
        std::cout << "Min Height: " << *std::min_element(finalHeightMap.begin(), finalHeightMap.end()) << std::endl;
        std::cout << "Max Height: " << *std::max_element(finalHeightMap.begin(), finalHeightMap.end()) << std::endl;
        std::cout << "Size: " << std::sqrt(finalHeightMap.size()) << "x" << std::sqrt(finalHeightMap.size()) << std::endl;

        return finalHeightMap;
    }

private:
    int m_initialSize;
    int m_targetSize;
    std::mt19937 m_rng;

    std::vector<float> GenerateLowResStartingImage() {
        std::vector<float> heightMap(m_initialSize * m_initialSize);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        for (float& height : heightMap) {
            height = dist(m_rng);
        }

        return heightMap;
    }

    std::vector<float> UpscaleHeightMap(const std::vector<float>& input) {
        int inputSize = static_cast<int>(std::sqrt(input.size()));
        int outputSize = inputSize * 2;
        std::vector<float> output(outputSize * outputSize);

        for (int y = 0; y < outputSize; ++y) {
            for (int x = 0; x < outputSize; ++x) {
                int inputX = x / 2;
                int inputY = y / 2;
                output[y * outputSize + x] = input[inputY * inputSize + inputX];
            }
        }

        return output;
    }

    std::vector<float> FinalBlur(const std::vector<float>& input) {
        std::vector<float> output = input;
        int size = static_cast<int>(std::sqrt(input.size()));

        for (int y = 1; y < size - 1; ++y) {
            for (int x = 1; x < size - 1; ++x) {
                float sum = 0.0f;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        sum += input[(y + dy) * size + (x + dx)];
                    }
                }
                output[y * size + x] = sum / 9.0f;
            }
        }

        return output;
    }

    void NormalizeAndEnhanceContrast(std::vector<float>& heightMap) {
        float minHeight = *std::min_element(heightMap.begin(), heightMap.end());
        float maxHeight = *std::max_element(heightMap.begin(), heightMap.end());
        float range = maxHeight - minHeight;

        if (range < 0.001f) {
            std::cout << "Warning: Very small height range detected. Adjusting..." << std::endl;
            for (float& height : heightMap) {
                height += static_cast<float>(m_rng()) / static_cast<float>(m_rng.max());
            }
            minHeight = *std::min_element(heightMap.begin(), heightMap.end());
            maxHeight = *std::max_element(heightMap.begin(), heightMap.end());
            range = maxHeight - minHeight;
        }

        for (float& height : heightMap) {
            // Normalize
            height = (height - minHeight) / range;

            // Enhance contrast
            height = std::pow(height, 1.5f);
        }
    }

    void AddDetails(std::vector<float>& blurry, const std::vector<float>& crisp) {
        for (size_t i = 0; i < blurry.size(); ++i) {
            blurry[i] += (crisp[i] - blurry[i]) * 0.5f; // Adjust the factor to control the amount of detail added
        }
    }

    void ApplyFormula(std::vector<float>& heightMap) {
        for (float& height : heightMap) {
            height = 1 - 1 / (1 + height);
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
        outputWidth(650), outputDepth(650), octaveCount(8), persistence(0.5f), heightMultiplier(0.20f),
        gradientFactor(5.03f), useDLA(false), dlaInitialSize(16), dlaTargetSize(256)
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
    DLATerrainGenerator dlaGenerator;
    std::vector<float> dlaTerrain;

    // Buffer Stuff - for perlin noise plane and the perlin noise + gradient plane
    GLuint vao[2]{}, vbo[2]{}, ebo[2]{};
    std::vector<Vertex> vertices[2];
    std::vector<unsigned int> indices[2];
};