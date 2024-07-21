#include <glhelper.h>
#include <../Input.h>
#include <glm/gtc/type_ptr.hpp>
#include <../KeyCodes.h>
#include "../imgui-1.87/imgui-1.87/imgui.h"
#include "../imgui-1.87/imgui-1.87/backends/imgui_impl_glfw.h"
#include "../imgui-1.87/imgui-1.87/backends/imgui_impl_opengl3.h"
#include "../PerlinNoise.hpp"
#include <glm/gtc/noise.hpp>

void PerlinNoise::load()
{
    // Vertex shader
    const std::string vert_Shader = R"(
        #version 450 core
        layout (location = 0) in vec3 aVertexPosition;
        layout (location = 1) in vec3 aVertexColor;
        uniform mat4 mvpMatrix;
        out vec3 FragPos;
        out vec3 Color;
        void main(void) {
          gl_Position = mvpMatrix * vec4(aVertexPosition, 1.0);
          FragPos = aVertexPosition;
          Color = aVertexColor;
        }
   )";

    // Fragment shader
    const std::string frag_Shader = R"(
        #version 450 core
        in vec3 FragPos;
        in vec3 Color;
        out vec4 fFragColor;
        uniform vec3 uLightPos;
        uniform vec3 uViewPos;
        void main() {
          vec3 normal = normalize(cross(dFdx(FragPos), dFdy(FragPos)));
          vec3 lightDir = normalize(uLightPos - FragPos);
          float diff = max(dot(normal, lightDir), 0.0);
          vec3 viewDir = normalize(uViewPos - FragPos);
          vec3 reflectDir = reflect(-lightDir, normal);
          float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
          vec3 ambient = 0.1 * Color;
          vec3 diffuse = diff * Color;
          vec3 specular = 0.2 * spec * vec3(1.0, 1.0, 1.0);
          fFragColor = vec4(ambient + diffuse + specular, 1.0);
      }
  )";
    shdr_pgm.CompileShaderFromString(GL_VERTEX_SHADER, vert_Shader);
    shdr_pgm.CompileShaderFromString(GL_FRAGMENT_SHADER, frag_Shader);
    if (!shdr_pgm.Link() || !shdr_pgm.Validate()) {
        std::cout << "Shaders is not linked and/or not validated!" << std::endl;
        std::cout << shdr_pgm.GetLog() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    shdr_pgm.PrintActiveAttribs();
    shdr_pgm.PrintActiveUniforms();
}


void PerlinNoise::init()
{
    noiseSeed.resize(outputWidth * outputDepth);
    perlinNoise.resize(outputWidth * outputDepth);
    perlinNoiseWithGradient.resize(outputWidth * outputDepth);

    SeedingPerlinNoise2D();

    // Generate both types of Perlin noise
    FractalPerlinNoise2D(outputWidth, outputDepth, noiseSeed, octaveCount, scalingBias, perlinNoise);
    FractalPerlinNoise2DWithGradient(outputWidth, outputDepth, noiseSeed, octaveCount, scalingBias, perlinNoiseWithGradient);

    // Generate terrain for both planes
    GeneratePerlinNoiseTerrain(perlinNoise, vertices[0], indices[0]);
    GeneratePerlinNoiseTerrain(perlinNoiseWithGradient, vertices[1], indices[1]);

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

    // Camera movement
    float cameraSpeed = m_cameraSpeed * static_cast<float>(dt);
    glm::vec3 forward = glm::normalize(GLHelper::cameraFront);
    glm::vec3 right = glm::normalize(glm::cross(forward, GLHelper::cameraUp));
    glm::vec3 up = GLHelper::cameraUp;

    glm::vec3 position = camera.getPosition();

    if (RE_input::is_key_pressed(RE_W)) {
        position += cameraSpeed * forward;
    }
    if (RE_input::is_key_pressed(RE_S)) {
        position -= cameraSpeed * forward;
    }
    if (RE_input::is_key_pressed(RE_A)) {
        position -= cameraSpeed * right;
    }
    if (RE_input::is_key_pressed(RE_D)) {
        position += cameraSpeed * right;
    }
    if (RE_input::is_key_pressed(RE_SPACE)) {
        position += cameraSpeed * up;
    }
    if (RE_input::is_key_pressed(RE_LEFT_SHIFT)) {
        position -= cameraSpeed * up;
    }

    camera.setPosition(position);

    // ImGui Stuff
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Perlin Noise Controls");

    // Common controls
    if (ImGui::SliderInt("Octave Count", &octaveCount, 1, 8))
    {
        RegeneratePerlinNoise();
    }

    if (ImGui::SliderFloat("Scaling Bias", &scalingBias, 1.0f, 3.0f, "%.2f"))
    {
        RegeneratePerlinNoise();
    }

    if (ImGui::SliderFloat("Height Multiplier", &heightMultiplier, 0.1f, 5.0f, "%.2f"))
    {
        RegeneratePerlinNoise();
    }

    // Gradient factor
    if (ImGui::SliderFloat("Gradient Factor", &gradientFactor, 0.0f, 1.0f, "%.2f"))
    {
        RegeneratePerlinNoise();
    }

    if (ImGui::Button("Regenerate Both Terrains"))
    {
        SeedingPerlinNoise2D();
        RegeneratePerlinNoise();
    }

    ImGui::Separator();

    // Individual plane controls
    const char* planeNames[] = { "Standard Perlin Noise", "Perlin Noise with Gradient" };
    for (int i = 0; i < 2; ++i)
    {
        if (ImGui::TreeNode(planeNames[i]))
        {
            bool visible = true; 
            if (ImGui::Checkbox("Visible", &visible))
            {
                // Empty for now. But can add plane specific stuff here for what planes we one.
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();

	// Perlin Noise Visualization window using Imgui
    ImGui::Begin("Perlin Noise Visualization");
	// Size of window to display the noise texture
    ImVec2 previewSize(400, 400);
    ImGui::Text("Perlin Noise Preview");

    // Create a texture for the noise
    static GLuint noiseTextureID = 0;
    if (noiseTextureID == 0)
    {
        glGenTextures(1, &noiseTextureID);
        glBindTexture(GL_TEXTURE_2D, noiseTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Update the texture with the current Perlin noise data
    std::vector<unsigned char> noiseData(outputWidth * outputDepth * 4); 
    for (int i = 0; i < outputWidth * outputDepth; ++i)
    {
        unsigned char value = static_cast<unsigned char>(perlinNoise[i] * 255.0f);
        noiseData[i * 4 + 0] = value; // R
        noiseData[i * 4 + 1] = value; // G
        noiseData[i * 4 + 2] = value; // B
        noiseData[i * 4 + 3] = 255;   // A (fully opaque)
    }

    glBindTexture(GL_TEXTURE_2D, noiseTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outputWidth, outputDepth, 0, GL_RGBA, GL_UNSIGNED_BYTE, noiseData.data());

    // Display the noise texture
    ImGui::Image((void*)(intptr_t)noiseTextureID, previewSize);

    ImGui::End();

    // Call draw function to draw the scene
	draw();
}

void PerlinNoise::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    shdr_pgm.Use();

    // Set up camera
    camera.setTarget(camera.getPosition() + GLHelper::cameraFront);
    camera.setFOV(GLHelper::fov);

    // Set up model, view, projection matrices
    glm::mat4 projection = camera.getProjectionMatrix();
    glm::mat4 view = camera.getViewMatrix();

    // Set uniforms
    shdr_pgm.SetUniform("uLightPos", glm::vec3(5.0f, 5.0f, 5.0f));
    shdr_pgm.SetUniform("uViewPos", camera.getPosition());

    // Draw both planes
    for (int i = 0; i < 2; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);

        // Offset the second plane
        if (i == 1) {
            model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
        }

        glm::mat4 mvp = projection * view * model;
        shdr_pgm.SetUniform("mvpMatrix", mvp);

        glBindVertexArray(vao[i]);
        glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    shdr_pgm.UnUse();

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
void PerlinNoise::SeedingPerlinNoise2D()
{
	// Seeding the noise array with random values
	for (int i = 0; i < outputWidth * outputDepth; i++)
		noiseSeed[i] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

}

void PerlinNoise::FractalPerlinNoise2D(int width, int height, const std::vector<float>& seed, int octaves, float bias, std::vector<float>& output)
{

	// Loop through each pixel
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        {
            float scaleAccumulate = 0.0f; // Accumulates the scaling factor for the curent pixel
			float noise    = 0.0f; // Accumulates the noise value for current pixel
			float scale    = 1.0f; // Scaling factor for the current pixel

            for (int i = 0; i < octaves; i++)
            {
				int pitch    = width >> i;           // Is the distance between the sample points
				int sampleX1 = (x / pitch) * pitch; // The x-coordinate of the top-left sample point
				int sampleY1 = (y / pitch) * pitch; // The y-coordinate of the top-left sample point

				int sampleX2 = (sampleX1 + pitch) % width; // The x-coordinate of the top-right sample point
				int sampleY2 = (sampleY1 + pitch) % width; // The y-coordinate of the bottom-left sample point

				float blendX = static_cast<float>((x - sampleX1)) / static_cast<float>(pitch); // The blend factor in x-direction
				float blendY = static_cast<float>((y - sampleY1)) / static_cast<float>(pitch); // The blend factor in y-direction

				float sampleT = (1.0f - blendX) * seed[sampleY1 * width + sampleX1] + blendX * seed[sampleY1 * width + sampleX2]; // Interpolated top sample
				float sampleB = (1.0f - blendX) * seed[sampleY2 * width + sampleX1] + blendX * seed[sampleY2 * width + sampleX2]; // Interpolated bottom sample

				scaleAccumulate += scale; // Accumulate the scaling factor
				noise  += (blendY * (sampleB - sampleT) + sampleT) * scale; // Add the interpolated noise to the result
				scale = scale / bias; // Scale down the scaling factor
            }

			output[y * width + x] = noise / scaleAccumulate; // Normalize and store the result in the 2d vector
        }
}

void PerlinNoise::GeneratePerlinNoiseTerrain(const std::vector<float>& noiseData, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices)
{
    outVertices.clear();
    outIndices.clear();

    float minHeight = std::numeric_limits<float>::max();
    float maxHeight = std::numeric_limits<float>::lowest();

    // First pass: find min and max heights
    for (int z = 0; z < outputDepth; z++)
    {
        for (int x = 0; x < outputWidth; x++)
        {
            float y   = noiseData[z * outputWidth + x] * heightMultiplier;
            minHeight = std::min(minHeight, y);
            maxHeight = std::max(maxHeight, y);
        }
    }

    // Define water level
    float waterLevel = minHeight + (maxHeight - minHeight) * 0.3f;

    // Define color palettes
    glm::vec3 waterColor(0.0f, 0.2f, 0.8f);
    glm::vec3 sandColor(0.76f, 0.7f, 0.5f);
    glm::vec3 grassColor(0.0f, 0.5f, 0.0f);
    glm::vec3 rockColor(0.5f, 0.5f, 0.5f);
    glm::vec3 snowColor(1.0f, 1.0f, 1.0f);

    // Simple pseudo-random function
    auto noise = [](float x, float y) {
        return glm::fract(sin(x * 12.9898f + y * 78.233f) * 43758.5453f);
        };

    // Second pass: generate vertices with colors based on the height
    for (int z = 0; z < outputDepth; z++)
    {
        for (int x = 0; x < outputWidth; x++)
        {
            float y = noiseData[z * outputWidth + x] * heightMultiplier;

            glm::vec3 position(x / static_cast<float>(outputWidth - 1) - 0.5f, y, z / static_cast<float>(outputDepth - 1) - 0.5f);

            float normalizedHeight = (y - minHeight) / (maxHeight - minHeight);

            glm::vec3 color;
            if (y <= waterLevel)
            {
                color = waterColor;
                position.y = waterLevel;
            }
            else if (normalizedHeight < 0.35f)
            {
                float t = (normalizedHeight - 0.3f) / 0.05f;
                color = glm::mix(sandColor, grassColor, t);
            }
            else if (normalizedHeight < 0.7f)
            {
                color = grassColor;
            }
            else if (normalizedHeight < 0.9f)
            {
                float t = (normalizedHeight - 0.7f) / 0.2f;
                color = glm::mix(grassColor, rockColor, t);
            }
            else
            {
                float t = (normalizedHeight - 0.9f) / 0.1f;
                color = glm::mix(rockColor, snowColor, t);
            }

            float colorNoise = noise(x * 0.1f, z * 0.1f) * 0.1f - 0.05f;
            color += glm::vec3(colorNoise);
            color = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));

            outVertices.push_back({ position, color });
        }
    }

    // Generate indices
    for (int z = 0; z < outputDepth - 1; z++)
    {
        for (int x = 0; x < outputWidth - 1; x++)
        {
            int topLeft     = z * outputWidth + x;
            int topRight    = topLeft + 1;
            int bottomLeft  = (z + 1) * outputWidth + x;
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

void PerlinNoise::FractalPerlinNoise2DWithGradient(int width, int height, const std::vector<float>& seed, int octaves, float bias, std::vector<float>& output)
{
    output.resize(width * height);
    gradient2D.resize(width * height);

    // First pass: generate base noise and calculate initial gradients
    for (int x = 0; x < width; x++)
    {
        for (int z = 0; z < height; z++)
        {
            float noise = 0.0f;
            float scaleAccumulate = 0.0f;
            float scale = 1.0f;

            for (int i = 0; i < octaves; i++)  
            {
                int pitch = width >> i;
                int sampleX1  = (x / pitch) * pitch;
                int sampleZ1  = (z / pitch) * pitch;
                int sampleX2  = (sampleX1 + pitch) % width;
                int sampleZ2  = (sampleZ1 + pitch) % height;
                float blendX  = (float)(x - sampleX1) / (float)pitch;
                float blendZ  = (float)(z - sampleZ1) / (float)pitch;
                float sampleT = (1.0f - blendX) * seed[sampleZ1 * width + sampleX1] + blendX * seed[sampleZ1 * width + sampleX2];
                float sampleB = (1.0f - blendX) * seed[sampleZ2 * width + sampleX1] + blendX * seed[sampleZ2 * width + sampleX2];
                scaleAccumulate += scale;
                noise += (blendZ * (sampleB - sampleT) + sampleT) * scale;
                scale = scale / bias;  // Apply bias within the octave loop
            }
            output[z * width + x] = noise / scaleAccumulate;
        }
    }

    // Calculate initial gradients
    for (int x = 0; x < width; x++)
    {
        for (int z = 0; z < height; z++)
        {
            gradient2D[z * width + x] = CalculateGradient(x, z, width, height, output);
        }
    }

    // Subsequent passes: add detail with gradient influence
    for (int o = 1; o < octaves; o++)
    {
        int pitch = width >> o;
        float scale = 1.0f;
        for (int i = 0; i < o; i++) scale /= bias;  // Apply bias cumulatively

        for (int x = 0; x < width; x++)
        {
            for (int z = 0; z < height; z++)
            {                 
                int sampleX1  = (x / pitch) * pitch;
                int sampleZ1  = (z / pitch) * pitch;
                int sampleX2  = (sampleX1 + pitch) % width;
                int sampleZ2  = (sampleZ1 + pitch) % height;
                float blendX  = (float)(x - sampleX1) / (float)pitch;
                float blendZ  = (float)(z - sampleZ1) / (float)pitch;
                float sampleT = (1.0f - blendX) * seed[sampleZ1 * width + sampleX1] + blendX * seed[sampleZ1 * width + sampleX2];
                float sampleB = (1.0f - blendX) * seed[sampleZ2 * width + sampleX1] + blendX * seed[sampleZ2 * width + sampleX2];
                float noise = (blendZ * (sampleB - sampleT) + sampleT) * scale;

                // Apply gradient influence
                glm::vec2 gradient = gradient2D[z * width + x];
                float gradientMagnitude = glm::length(gradient);
                float gradientInfluence = 1.0f - glm::min(gradientMagnitude * gradientFactor, 1.0f);
                output[z * width + x] += noise * gradientInfluence;

                // Update gradient
                gradient2D[z * width + x] = CalculateGradient(x, z, width, height, output);
            }
        }
    }
}
glm::vec2 PerlinNoise::CalculateGradient(int x, int z, int width, int depth, const std::vector<float>& noise)
{
    float epsilon      = 1.0f;
    float heightCenter = noise[z * width + x];
    float heightRight  = (x < width - 1) ? noise[z * width + (x + 1)] : heightCenter;
    float heightUp     = (z < depth - 1) ? noise[(z + 1) * width + x] : heightCenter;

    return glm::vec2((heightRight - heightCenter) / epsilon, (heightUp - heightCenter) / epsilon);
}

void PerlinNoise::RegeneratePerlinNoise()
{
    FractalPerlinNoise2D(outputWidth, outputDepth, noiseSeed, octaveCount, scalingBias, perlinNoise);
    FractalPerlinNoise2DWithGradient(outputWidth, outputDepth, noiseSeed, octaveCount, scalingBias, perlinNoiseWithGradient);

    GeneratePerlinNoiseTerrain(perlinNoise, vertices[0], indices[0]);
    GeneratePerlinNoiseTerrain(perlinNoiseWithGradient, vertices[1], indices[1]);

    // Update GPU buffers
    for (int i = 0; i < 2; ++i)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
        glBufferData(GL_ARRAY_BUFFER, vertices[i].size() * sizeof(Vertex), vertices[i].data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(unsigned int), indices[i].data(), GL_STATIC_DRAW);
    }
}