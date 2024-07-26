
#include "PerlinNoise.hpp"

#include "Input.h"
#include "KeyCodes.h"
#include "glhelper.h"

#include "camera.hpp"


void PerlinNoise::control(double dt) {
    /*
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
    */

    if (RE_input::is_key_pressed(GLFW_KEY_W)) {
        main_camera.on_move(1, dt);
        //map_camera.follow_map(main_camera, true);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_S)) {
        main_camera.on_move(-1, dt);
        //map_camera.follow_map(main_camera, true);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_R) || RE_input::is_key_pressed(GLFW_KEY_SPACE)) {
        main_camera.on_move(5, dt);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_F) || RE_input::is_key_pressed(GLFW_KEY_LEFT_SHIFT)) {
        main_camera.on_move(4, dt);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_A)) {
        main_camera.on_move(2, dt);
        //map_camera.follow_map(main_camera, true);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_D)) {
        main_camera.on_move(3, dt);
        //map_camera.follow_map(main_camera, true);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_E)) {
        main_camera.on_move(6, dt);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_Q)) {
        main_camera.on_move(7, dt);
    }
}