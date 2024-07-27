/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
Course:     CS350
Project:    Assignment 1
Filename:   camera.hpp
Author(s):  Ang Wei Jie <weijie.ang@digipen.edu>
Date:       16/06/2024

Contents:
    - Handling of 3D camera with main support for perspective and minor support for
      ortho.
====================================================================================*/

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp> // vec2, vec3, mat4, radians
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

enum class camera_type {
    persp,
    ortho
};

class camera {
private:
    camera_type type;
	glm::vec3 position;
	glm::vec3 target;
    float pitch;
    float yaw;
	float field_of_view;
	float near;
	float far;
    float sensitivity;
    float move_speed;
    float turn_speed;
    bool inverse;
    bool flight;

    // for ortho only
    float left;
    float right;
    float bottom;
    float top;

public:
    camera(
        camera_type type = camera_type::persp,
        glm::vec3 position = { 0.0f, 1.0f, 10.0f },
        glm::vec3 target = { 0.0f, 0.0f, 0.0f },
        float fieldOfView = 45.0f,
        float left = -10.0f,
        float right = 10.0f,
        float bottom = -10.0f,
        float top = 10.0f,
        float near = 0.0001f,
        float far = 200.f
    ) :
        type{ type },
        position{ position },
        target{ target },
        field_of_view{ fieldOfView },
        near{ near },
        far{ far },
        pitch{ glm::asin((target.y - position.y) / glm::length(target - position)) },
        yaw{ std::atan2f((target.x - position.x), (target.z - position.z)) },
        sensitivity{ 1.f },
        inverse{ false },
        flight{ true },
        move_speed{ 2.5f },
        turn_speed{ glm::radians(25.f) },
        left{ left },
        right{ right },
        bottom{ bottom },
        top{ top }
    { }

    float& get_move_speed() {
        return move_speed;
    }

    float& get_turn_speed() {
        return turn_speed;
    }

    float& get_sensitivity() {
        return sensitivity;
    }

    bool& get_inverse() {
        return inverse;
    }

    bool& get_flight() {
        return flight;
    }

    glm::vec3 const& get_position() const {
        return position;
    }
    
    void change_ortho_scale(float new_scale) {
        if (type == camera_type::ortho) {
            left = -new_scale;
            right = new_scale;
            bottom = -new_scale;
            top = new_scale;

        }
    }

    glm::mat4 get_look_at(glm::vec3 center = { 0.0f, 0.0f, 0.0f }, glm::vec3 up = { 0.0f, 1.0f, 0.0f }) const
    {
        return glm::lookAt(position, target, up);
    }

    glm::mat4 get_projection(float aspect = 1.0f) const
    {
        if (type == camera_type::persp) {
            return glm::perspective(glm::radians(field_of_view), aspect, near, far);
        }
        else {
            float ortho_width = (right - left) / 2.0f;
            float ortho_height = (top - bottom) / 2.0f;
            if (aspect > 1.0f) {
                ortho_width *= aspect; // wider than tall
            } else {
                ortho_height /= aspect; // taller than wide
            }

            return glm::ortho(-ortho_width, ortho_width, -ortho_height, ortho_height, near, far);
        }
    }

    void update_camera_yaw() {
        if (type == camera_type::persp) {
            glm::vec3 front;
            front.x = cos(pitch) * sin(yaw);
            front.y = sin(pitch);
            front.z = cos(pitch) * cos(yaw);
            target = glm::normalize(front) + position;
        }
    }

    void on_move(int offset, float dt) {
        // offset
        // -1 / 1 = backward / forward
        // 2 / 3 = left /  right
        // 4 / 5 = down / up
        // 6 / 7 = turn left / right

        glm::vec3 velocity;
        float adjusted_speed = move_speed * dt;

        if (type == camera_type::persp) {
            if (offset <= 1) { // backward & forward
                velocity = glm::normalize(offset > 0 ? target - position : position - target);
                if (!flight) {
                    velocity = glm::vec3(velocity.x, 0.f, velocity.z);
                }
                position += velocity * adjusted_speed;
                target += velocity * adjusted_speed;
            }
            else if (offset == 2) { // left
                glm::vec3 direction = glm::normalize(target - position);
                velocity = -glm::normalize(glm::cross(direction, glm::vec3(0, 1, 0)));
                position += velocity * adjusted_speed;
                target += velocity * adjusted_speed;
            }
            else if (offset == 3) { // right
                glm::vec3 direction = glm::normalize(target - position);
                velocity = glm::normalize(glm::cross(direction, glm::vec3(0, 1, 0)));
                position += velocity * adjusted_speed;
                target += velocity * adjusted_speed;
            }
            else if (offset == 4) { // down
                velocity = glm::normalize(glm::vec3(0.f, -1.f, 0.f));
                position += velocity * adjusted_speed;
                target += velocity * adjusted_speed;
            }
            else if (offset == 5) { // up
                velocity = glm::normalize(glm::vec3(0.f, 1.f, 0.f));
                position += velocity * adjusted_speed;
                target += velocity * adjusted_speed;
            }
            else if (offset == 6) { // turn left
                yaw -= turn_speed * dt;
                update_camera_yaw();
            }
            else if (offset == 7) { // turn right
                yaw += turn_speed * dt;
                update_camera_yaw();
            }
        }
        else if (type == camera_type::ortho) {
            if (offset == 1) { // forward (decrease z)
                position.z -= adjusted_speed;
                target.z -= adjusted_speed;
            }
            else if (offset == -1) { // backward (increase z)
                position.z += adjusted_speed;
                target.z += adjusted_speed;
            }
            else if (offset == 2) { // left (decrease x)
                position.x -= adjusted_speed;
                target.x -= adjusted_speed;
            }
            else if (offset == 3) { // right (increase x)
                position.x += adjusted_speed;
                target.x += adjusted_speed;
            }
            else if (offset == 4) { // down (decrease y)
                position.y -= adjusted_speed;
                target.y -= adjusted_speed;
            }
            else if (offset == 5) { // up (increase y)
                position.y += adjusted_speed;
                target.y += adjusted_speed;
            }
        }
    }

    void on_cursor(float xoffset, float yoffset) {
        if (type != camera_type::persp) return;
        
        float cam_sensitivity = 0.002f * sensitivity;
        constexpr float PI05 = glm::pi<float>() / 2.0f;

        if (inverse) {
            yaw += xoffset * cam_sensitivity;
            pitch += yoffset * cam_sensitivity;
        }
        else {
            yaw -= xoffset * cam_sensitivity;
            pitch -= yoffset * cam_sensitivity;
        }

        if (pitch > PI05 - 0.01f) pitch = PI05 - 0.01f;
        if (pitch < -PI05 + 0.01f) pitch = -PI05 + 0.01f;

        update_camera_yaw();
    }

    void on_scroll(float xoffset, float yoffset) {
        if (type != camera_type::persp) return;
        
        constexpr float zoom_speed = 2.0f;
        field_of_view -= yoffset * zoom_speed;

        // clamp the FOV to prevent extreme zoom
        if (field_of_view < 1.0f) field_of_view = 1.0f;
        if (field_of_view > 130.0f) field_of_view = 130.0f;
    }

    void follow_map(camera const& other_camera, bool float_movement) {
        if (float_movement) {
            position.x = other_camera.position.x;
            target.x = other_camera.position.x;

            position.z = other_camera.position.z;
            target.z = other_camera.position.z;
        }
        else {
            position.x = other_camera.position.x;
            target.x = other_camera.target.x;

            position.z = other_camera.position.z;
            target.z = other_camera.target.z;
        }
    }
};

// camera objects
extern camera main_camera; // main freecam
extern camera map_camera; // map view cam

#endif