#pragma once
/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
Filename:   Input.h
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
#include "Input.h"
#include "glhelper.h"

class RE_input {
public:
    // Mouse
    static bool is_mouse_button_pressed(int button);
    static bool is_mouse_button_released(int button);
    static float get_mouse_x();
    static float get_mouse_y();

    // Keyboard
    static bool is_key_pressed(int keycode);
    static bool is_key_released(int keycode);

private:
    struct mouse_position {
        float x;
        float y;
    };
    static mouse_position get_mouse_position();
};






