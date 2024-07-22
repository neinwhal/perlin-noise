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


class PerlinNoise : public Scene_Base
{
public:
    PerlinNoise(Scene_Manager& sceneManager, const std::string& name)
        : _sceneManager(sceneManager), _name(name), elapsedTime(0.0f),
        m_cameraSpeed(2.5f), m_cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), m_cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
        camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, (float)GLHelper::width / (float)GLHelper::height, 0.1f, 100.0f),
        outputWidth(512), outputDepth(512), octaveCount(8), persistence(0.5f), heightMultiplier(0.20f),
        gradientFactor(5.03f)
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
    static float Fade(float t);
    static float Lerp(float t, float a, float b);
    static float Grad(int hash, float x, float y, float z);

    void GeneratePerlinNoise();
    void GeneratePerlinNoiseWithGradient();
    void GeneratePerlinNoiseTerrain(const std::vector<float>& noiseData, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices);
    glm::vec2 CalculateGradient(int x, int z, int width, int depth, const std::vector<float>& noise);

    // Regenerate
    void RegeneratePerlinNoise();

    // Buffer Stuff - for perlin noise plane and the perlin noise + gradient plane
    GLuint vao[2]{}, vbo[2]{}, ebo[2]{};
    std::vector<Vertex> vertices[2];
    std::vector<unsigned int> indices[2];



};