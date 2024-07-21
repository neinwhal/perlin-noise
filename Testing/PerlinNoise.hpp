#pragma once
#include "glapp.h"
#include "./Camera.h"
#include "./Primitives.h"
#include "./IntersectionTest.h"

class PerlinNoise : public Scene_Base
{
public:
    PerlinNoise(Scene_Manager& sceneManager, const std::string& name)
        : _sceneManager(sceneManager), _name(name), elapsedTime(0.0f),
        m_cameraSpeed(2.5f), m_cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), m_cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
        camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, (float)GLHelper::width / (float)GLHelper::height, 0.1f, 100.0f),
        outputWidth(128), outputDepth(128), octaveCount(1), scalingBias(2.0f), heightMultiplier(2.0f),
        gradientFactor(0.5f)
    {
        // Empty by design/choice 
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
    std::vector<float> noiseSeed;
    std::vector<float> perlinNoise;
    std::vector<float> perlinNoiseWithGradient;
    int octaveCount;
    float scalingBias;
    void SeedingPerlinNoise2D();
    void FractalPerlinNoise2D(int width, int height, const std::vector<float>& seed, int octaves, float bias, std::vector<float>& output);
    void GeneratePerlinNoiseTerrain(const std::vector<float>& noiseData, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices);
    
    // Gradient Stuff
    float gradientFactor;
    std::vector<glm::vec2> gradient2D;
    float heightMultiplier;
    void FractalPerlinNoise2DWithGradient(int width, int height, const std::vector<float>& seed, int octaves, float bias, std::vector<float>& output);
    glm::vec2 CalculateGradient(int x, int z, int width, int depth, const std::vector<float>& noise);
 
    // Regenerate
    void RegeneratePerlinNoise();

	// Buffer Stuff - for perlin noise plane and the perlin noise + gradient plane
    GLuint vao[2]{}, vbo[2]{}, ebo[2]{};
    std::vector<Vertex> vertices[2];
    std::vector<unsigned int> indices[2];
};