
#include "Noise.hpp"

#include "Input.h"
#include "KeyCodes.h"
#include "glhelper.h"

#include "camera.hpp"


void PerlinNoise::control(float dt) {
    if (RE_input::is_key_pressed(GLFW_KEY_W)) {
        main_camera.on_move(1, dt);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_S)) {
        main_camera.on_move(-1, dt);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_R) || RE_input::is_key_pressed(GLFW_KEY_SPACE)) {
        main_camera.on_move(5, dt);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_F) || RE_input::is_key_pressed(GLFW_KEY_LEFT_SHIFT)) {
        main_camera.on_move(4, dt);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_A)) {
        main_camera.on_move(2, dt);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_D)) {
        main_camera.on_move(3, dt);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_E)) {
        main_camera.on_move(6, dt);
    }
    if (RE_input::is_key_pressed(GLFW_KEY_Q)) {
        main_camera.on_move(7, dt);
    }
}