#include "Input.h"
#include "KeyCodes.h"
/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
Filename:   Input.cpp
Project:    Untitled RTS <Engine/Game>
Author(s):  Bjorn Pokin Chinnaphongse <bjornpokin.c@digipen.edu> (primary: 100 %)

Contents:
    This class provides static methods to handle keyboard and mouse events.
    It offers functions to check the state of a specific keyboard key or mouse button.
    It also provides mouse position retrieval functionality.

    Example Usage:
        if (RE_input::is_key_pressed(RE_A))
        {
            // Do something when 'A' is pressed
        }

    float x = RE_input::get_mouse_x();
    float y = RE_input::get_mouse_y();

====================================================================================*/

// Mouse
RE_input::mouse_position RE_input::get_mouse_position() 
{
    double x_position, y_position;
    glfwGetCursorPos(GLHelper::ptr_window, &x_position, &y_position);
    return { static_cast<float>(x_position), static_cast<float>(y_position) };
}

float RE_input::get_mouse_x() 
{
    return get_mouse_position().x;
}

float RE_input::get_mouse_y() 
{
    return get_mouse_position().y;
}

bool RE_input::is_mouse_button_pressed(int button) 
{
    int state = glfwGetMouseButton(GLHelper::ptr_window, button);
    return state == GLFW_PRESS ? true : false;
}

bool RE_input::is_mouse_button_released(int button)
{
    int state = glfwGetMouseButton(GLHelper::ptr_window, button);
    return state == GLFW_RELEASE ? true : false;
}

// Keyboard
bool RE_input::is_key_pressed(int keycode) 
{
    int state = glfwGetKey(GLHelper::ptr_window, keycode);
    return state == GLFW_PRESS ? true : state == GLFW_REPEAT ? true : false;
}
bool RE_input::is_key_released(int keycode) 
{
    int state = glfwGetKey(GLHelper::ptr_window, keycode);
    return state == GLFW_RELEASE ? true : false;
}




