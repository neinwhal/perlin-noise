/* !
@file		glhelper.h
@author		pghali@digipen.edu
@co-author	bjornpokin.c@digipen.edu
@date		06/05/2022

This file contains the declaration of namespace Helper that encapsulates the
functionality required to create an OpenGL context using GLFW; use GLEW
to load OpenGL extensions; initialize OpenGL state; and finally initialize
the OpenGL application by calling initalization functions associated with
objects participating in the application.

*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLHELPER_H
#define GLHELPER_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <GL/glew.h> // for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <iostream>
#include <glslshader.h>
#include <sstream>
#include <iomanip>
#include <glapp.h>
#include <../Input.h>

/*  _________________________________________________________________________ */
struct GLHelper
  /*! GLHelper structure to encapsulate initialization stuff ...
  */
{
  // take a referecne to Event and returns void
  //using EventcallbackFn = std::function<void(Event&)>;
  //static std::function<void(Event&)> EventCallback;
  //static void SetEventCallback(const std::function<void(Event&)>& callback)
  //{
  //    EventCallback = callback;
  //}

  static bool init(GLint w, GLint h, std::string t);
  static void cleanup();
  //TODO: print_spec
  static void print_specs();

  // callbacks ...
  static void error_cb(int error, char const* description);
  static void fbsize_cb(GLFWwindow *ptr_win, int width, int height);
 
  // I/O callbacks ...
  //static void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod);
  //static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod);
  //static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset);
  //static void mousepos_cb(GLFWwindow *pwin, double xpos/*, double ypos*/);

  static void update_time(double fpsCalcInt = 1.0);

  static int width, height;
  static GLdouble fps;
  static GLdouble delta_time; // time taken to complete most recent game loop
  static std::string title;
  static GLFWwindow *ptr_window;

  static float lastX;
  static float lastY;
  static bool firstMouse;
  static float yaw;
  static float pitch;
  static glm::vec3 cameraFront;
  static glm::vec3 cameraUp;
  static float fov;
  static bool rightMouseButtonPressed;

  //class WindowsInput : public Input
  //{
  //protected:
  //    virtual bool IsKeyPressedImpl(int keycode) override;
  //};

};

#endif /* GLHELPER_H */
