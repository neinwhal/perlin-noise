
#include "PerlinNoise.hpp"

#include "Input.h"
#include "KeyCodes.h"
#include "glhelper.h"

void PerlinNoise::control(double dt) {
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
}