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

enum terrain_types {
    PERLIN_NOISE,
    GRAIDENT_NOISE,
    DLA_NOISE
};

class PerlinNoise : public Scene_Base {
public:
    PerlinNoise(Scene_Manager& sceneManager, const std::string& name)
        : _sceneManager(sceneManager), _name(name), elapsedTime(0.0f),
        visualization_primitive(0), light_position(glm::vec3(5.f, 5.f, 5.f)),
        outputWidth(650), outputDepth(650), octaveCount(8), persistence(0.5f), heightMultiplier(0.07f),
        gradientFactor(5.03f), useDLA(false)
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

    void control(float dt);
    void gui();

    // sub guis
    void gui_terrain_gen_controls();
    void gui_terrain_visualization();
    void gui_settings();



private:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
    };

    // Scene Manager
    std::string _name;
    Scene_Manager& _sceneManager;
    double elapsedTime;

    // Lighting
    int visualization_primitive;
    glm::vec3 light_position;

    // Perlin Noise Stuff
    int outputWidth, outputDepth;
    std::vector<float> perlinNoise;
    std::vector<float> perlinNoiseWithGradient;
    int octaveCount;
    float persistence;
    float heightMultiplier;
    float gradientFactor;
    std::vector<int> p;

    // DLA Noise Stuff
    struct dlaNode {
        float x, y;
        float parent_x, parent_y;
    };

    std::vector<std::vector<float>> dlaData;
    std::vector<std::vector<float>> blurry_dlaData;
	std::vector<float> flat_blurry_dlaData;
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

    void GeneratePerlinNoise();
    void GeneratePerlinNoiseWithGradient();
    void GeneratePerlinNoiseTerrain(const std::vector<float>& noiseData, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices);
    glm::vec2 CalculateGradient(int x, int z, int width, int depth, const std::vector<float>& noise);
    
	void GenerateTerrainDLA();
    void GenerateDLATerrain(int stage);
    bool useDLA;

    // Regenerate
    void RegenerateNoise();

    // Buffer Stuff - for perlin noise plane and the perlin noise + gradient plane
    GLuint vao[3]{}, vbo[3]{}, ebo[3]{};
    std::vector<Vertex> vertices[3];
    std::vector<unsigned int> indices[3];
};