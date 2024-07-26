/*!
@file    main.cpp
@author  pghali@digipen.edu
@date    10/11/2016

This file uses functionality defined in types GLHelper and GLApp to initialize 
an OpenGL context and implement a game loop.

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
// Extension loader library's header must be included before GLFW's header!!!
#include "../imgui-1.87/imgui-1.87/imgui.h"
#include "../imgui-1.87/imgui-1.87/backends/imgui_impl_glfw.h"
#include "../imgui-1.87/imgui-1.87/backends/imgui_impl_opengl3.h"
#include "glhelper.h"
#include "glapp.h"
#include <iostream>
#include <memory>
#include "PerlinNoise.hpp"

/*                                                   type declarations
----------------------------------------------------------------------------- */

/*                                                      function declarations
----------------------------------------------------------------------------- */
static void draw();
static void update();
static void init();
static void cleanup();
Scene_Manager sceneManager;

/*                                                      function definitions
----------------------------------------------------------------------------- */
/*  _________________________________________________________________________ */
/*! main

@param none

@return int

Indicates how the program existed. Normal exit is signaled by a return value of
0. Abnormal termination is signaled by a non-zero return value.
Note that the C++ compiler will insert a return 0 statement if one is missing.
*/
int main() {

   
  // Part 1
  init();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui::StyleColorsDark();

  // Initialize ImGui for GLFW and OpenGL
  ImGui_ImplGlfw_InitForOpenGL(GLHelper::ptr_window, true);
  ImGui_ImplOpenGL3_Init("#version 450");
  //auto ass1 = std::make_shared<Assignment1>(sceneManager, "ass1");
  //auto ass2 = std::make_shared<Assignment2>(sceneManager, "ass2");
  auto perlinNoise = std::make_shared<PerlinNoise>(sceneManager, "perlinNoise");
 // sceneManager.addScene("ass1", ass1);
  sceneManager.addScene("perlinNoise", perlinNoise);
  //sceneManager.activateScene("ass1");
  sceneManager.activateScene("perlinNoise");

  while (!glfwWindowShouldClose(GLHelper::ptr_window)) {
    // Part 2a
    update();
    // Part 2b
    draw();
  }

  // Part 3
  cleanup();
}

/*  _________________________________________________________________________ */
/*! update
@param none
@return none

Uses GLHelper::GLFWWindow* to get handle to OpenGL context.
For now, there are no objects to animate nor keyboard, mouse button click,
mouse movement, and mouse scroller events to be processed.
*/
static void update() {
    glfwPollEvents();

  // Part 2
  GLHelper::update_time(1.0);
  sceneManager.update(GLHelper::delta_time);
  // Part 1

  // Part 3
  //app.update();
}

/*  _________________________________________________________________________ */
/*! draw
@param none
@return none

Call application to draw and then swap front and back frame buffers ...
Uses GLHelper::GLFWWindow* to get handle to OpenGL context.
*/
static void draw() {
  // Part 1
  //app.draw();

  // Part 2: swap buffers: front <-> back
  glfwSwapBuffers(GLHelper::ptr_window);
}

/*  _________________________________________________________________________ */
/*! init
@param none
@return none

The OpenGL context initialization stuff is abstracted away in GLHelper::init.
The specific initialization of OpenGL state and geometry data is
abstracted away in GLApp::init
*/
static void init() {
  // Part 1
  if (!GLHelper::init(1300, 1300, "Assignment 2 - Bounding Volumes Representation ")) {
    std::cout << "Unable to create OpenGL context" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  // Part 2
  GLHelper::print_specs();

  // Part 3
  //app.init();
}

/*  _________________________________________________________________________ */
/*! cleanup
@param none
@return none

Return allocated resources for window and OpenGL context thro GLFW back
to system.
Return graphics memory claimed through 
*/
void cleanup() {
  // Part 1
  //app.cleanup();
	sceneManager.terminate();
  // Part 2
  GLHelper::cleanup();
}
