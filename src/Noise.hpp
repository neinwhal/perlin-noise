#pragma once
#include "glapp.h"
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
#define M_PI 3.14159265358979323846

enum terrain_types {
	PERLIN_NOISE, // 0
    ALT_PERLIN_NOISE, // 1
	GRAIDENT_NOISE, // 2
    ALT_GRAIDENT_NOISE, // 3
    DLA_NOISE
};

class PerlinNoise : public Scene_Base {
public:
    PerlinNoise(Scene_Manager& sceneManager, const std::string& name)
        : _sceneManager(sceneManager), _name(name), elapsedTime(0.0f),
        visualization_primitive(0), light_position(glm::vec3(5.f, 5.f, 5.f)),
        outputWidth(650), outputDepth(650), octaveCount(8), persistence(0.5f), heightMultiplier(0.07f),
        gradientFactor(5.03f), dlaLevel(0), terrain_gen_type(0)
    {
		perlinNoise.resize(outputWidth * outputDepth);
        altPerlinNoise.resize(outputWidth * outputDepth);
        perlinNoiseWithGradient.resize(outputWidth * outputDepth);
		altPerlinNoiseWithGradient.resize(outputWidth * outputDepth);
        InitializePermutationVector();
    }
    virtual void load() override;
    virtual void init() override;
    virtual void update(double dt) override;
    virtual void draw() override;
    virtual void cleanup() override;

    void control(float dt);
    void gui();

    // sub guis
    void gui_terrain_gen_controls();
    void gui_terrain_visualization();
    void gui_settings();

   

    float GenerateNoise(float x, float y, int outputWidth, int outputDepth, int octaves = 8) {
        float value = 0.0f;
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float maxValue = 0.0f;

        for (int i = 0; i < octaves; ++i) {
            value += perlin(x * frequency, y * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= 0.5f;
            frequency *= 2.0f;
        }

        // Normalize and scale the result
        value = value / maxValue;

        // Optional: Adjust the contrast
        value = (value * 2.0f) - 1.0f;  // Map to [-1, 1]
        value = (std::sin(value * static_cast<float>(M_PI) * 0.5f) + 1.0f) * 0.5f;  // Smooth curve and remap to [0, 1]

        return value;
    }

private:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
    };


    int visualization_primitive;
    int terrain_gen_type;

    // Scene Manager
    std::string _name;
    Scene_Manager& _sceneManager;
    double elapsedTime;

    // Lighting
    glm::vec3 light_position;

    // Perlin Noise Stuff
    int outputWidth, outputDepth;
	std::vector<float> perlinNoise;
	std::vector<float> perlinNoiseWithGradient;

    std::vector<float> altPerlinNoise;
    std::vector<float> altPerlinNoiseWithGradient;
    int octaveCount;
    float persistence;
    float heightMultiplier;
    float gradientFactor;
    std::vector<int> p;

    // DLA Noise Stuff
    struct dlaNode {
        float x, y;
        float parent_x, parent_y;
        float depth;
    };
    float dlaList_max_depth;

    std::vector<std::vector<float>> dlaData;
    std::vector<std::vector<float>> dlaData_depth;
    std::vector<std::vector<float>> blurry_dlaData;
	std::vector<float> flat_blurry_dlaData;

    int dlaLevel;

    std::vector<std::vector<float>> mountData;
    std::vector<dlaNode> dlaList;

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

     // Proper Perlin Noise
     struct Vector2 {
         float x, y;
     };

     float perlin(float x, float y) {
         int x0 = static_cast<int>(std::floor(x));
         int y0 = static_cast<int>(std::floor(y));
         int x1 = x0 + 1;
         int y1 = y0 + 1;

         float sx = smoothstep(x - static_cast<float>(x0));
         float sy = smoothstep(y - static_cast<float>(y0));

         float n0 = dotGridGradient(x0, y0, x, y);
         float n1 = dotGridGradient(x1, y0, x, y);
         float ix0 = std::lerp(n0, n1, sx);

         n0 = dotGridGradient(x0, y1, x, y);
         n1 = dotGridGradient(x1, y1, x, y);
         float ix1 = std::lerp(n0, n1, sx);

         return std::lerp(ix0, ix1, sy);
     }
     static Vector2 randomGradient(int ix, int iy) {
         const unsigned w = 8 * sizeof(unsigned);
         const unsigned s = w / 2;
         unsigned a = ix, b = iy;
         a *= 3284157443; b ^= (a << s) | (a >> (w - s));
         b *= 1911520717; a ^= (b << s) | (b >> (w - s));
         a *= 2048419325;
         float random = a * (3.14159265f / ~(~0u >> 1));
         return { std::cos(random), std::sin(random) };
     }

     float dotGridGradient(int ix, int iy, float x, float y) {
         Vector2 gradient = randomGradient(p[(p[ix & 255] + iy) & 255], p[(p[ix & 255] + iy + 1) & 255]);
         float dx = x - static_cast<float>(ix);
         float dy = y - static_cast<float>(iy);
         return (dx * gradient.x + dy * gradient.y);
     }

     static float smoothstep(float t) {
         return t * t * (3 - 2 * t);
     }

	void GeneratePerlinNoise();
	void GeneratePerlindNoiseWithGradient();
    void GenerateAltPerlinNoise();
    void GenerateAltPerlinNoiseWithGradient();
    void GeneratePerlinNoiseTerrain(const std::vector<float>& noiseData, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices);
    glm::vec2 CalculateGradient(int x, int z, int width, int depth, const std::vector<float>& noise);
    
    std::vector<float> flatten2DVector(const std::vector<std::vector<float>>& vec2D);
	void GenerateTerrainDLA();
    void FlattenTerrainDLA();
    void GenerateDLATerrain(int stage);

    // Regenerate
    void RegenerateNoise();

    // Buffer Stuff - for perlin noise plane and the perlin noise + gradient plane
    GLuint vao[5]{}, vbo[5]{}, ebo[5]{};
    std::vector<Vertex> vertices[5];
    std::vector<unsigned int> indices[5];
};