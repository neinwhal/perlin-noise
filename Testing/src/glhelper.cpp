/*!
@file       glhelper.cpp
@author		pghali@digipen.edu
@co-author	bjornpokin.c@digipen.edu
@date		06/05/2022

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and use GLEW to extract function 
pointers to OpenGL implementations.

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glhelper.h>



/*                                                   objects with file scope
----------------------------------------------------------------------------- */
// static data members declared in GLHelper
int GLHelper::width;
int GLHelper::height;
GLdouble GLHelper::fps;
GLdouble GLHelper::delta_time;
std::string GLHelper::title;
GLFWwindow* GLHelper::ptr_window;
//GLHelper::EventcallbackFn GLHelper::EventCallback;
static GLApp app;

float GLHelper::lastX = GLHelper::width / 2.0f;
float GLHelper::lastY = GLHelper::height / 2.0f;
bool GLHelper::firstMouse = true;
float GLHelper::yaw = -90.0f;
float GLHelper::pitch = 0.0f;
glm::vec3 GLHelper::cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 GLHelper::cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float GLHelper::fov = 45.0f;
bool GLHelper::rightMouseButtonPressed = false;
/*  _________________________________________________________________________ */
/*! init

@param GLint width
@param GLint height
Dimensions of window requested by program

@param std::string title_str
String printed to window's title bar

@return bool
true if OpenGL context and GLEW were successfully initialized.
false otherwise.

Uses GLFW to create OpenGL context. GLFW's initialization follows from here:
http://www.glfw.org/docs/latest/quick.html
a window of size width x height pixels
and its associated OpenGL context that matches a core profile that is 
compatible with OpenGL 4.5 and doesn't support "old" OpenGL, has 32-bit RGBA,
double-buffered color buffer, 24-bit depth buffer and 8-bit stencil buffer 
with each buffer of size width x height pixels
*/
bool GLHelper::init(GLint w, GLint h, std::string t) {
  GLHelper::width = w;
  GLHelper::height = h;
  GLHelper::title = t;

  // Part 1
  if (!glfwInit()) {
    std::cout << "GLFW init has failed - abort program!!!" << std::endl;
    return false;
  }

  //// Set up the event callback
  //GLHelper::EventCallback = [](Event& event) {
  //    //std::cout << "Event type: " << event.ToString() << std::endl;

  //    //dispatcher.Dispatch<WindowCloseEvent>(GLHelper::OnWindowClose);
  //};

  //Input::InitializeInstance(new GLHelper::WindowsInput());
  
  // In case a GLFW function fails, an error is reported to callback function
  glfwSetErrorCallback(GLHelper::error_cb);

  // Before asking GLFW to create an OpenGL context, we specify the minimum constraints
  // in that context:
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); // window dimensions are static

  GLHelper::ptr_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (!GLHelper::ptr_window) {
    std::cerr << "GLFW unable to create OpenGL context - abort program\n";
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(GLHelper::ptr_window);
  glfwSetWindowSizeCallback(GLHelper::ptr_window, [](GLFWwindow* window, int _width, int _height) {
	  static_cast<void>(window);
      GLHelper::width = _width;
      GLHelper::height = _height;

    //  WindowResizeEvent event(width, height);
    //  GLHelper::EventCallback(event);
      });

  glfwSetWindowCloseCallback(GLHelper::ptr_window, [](GLFWwindow* window) {
      static_cast<void>(window);
     // WindowCloseEvent event;
    //  GLHelper::EventCallback(event);
      });

  glfwSetFramebufferSizeCallback(GLHelper::ptr_window, GLHelper::fbsize_cb);
  glfwSetKeyCallback(GLHelper::ptr_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
      static_cast<void>(key);
	  static_cast<void>(mods);
      static_cast<void>(window);
      static_cast<void>(scancode);
      switch (action) {
          case GLFW_PRESS:
          {
             // KeyPressedEvent event(key, 0);
             // GLHelper::EventCallback(event);
              break;
          }
          case GLFW_RELEASE:
          {
             // KeyReleasedEvent event(key);
             // GLHelper::EventCallback(event);
              break;
          }
          case GLFW_REPEAT:
          {
             // KeyPressedEvent event(key, 1);
             // GLHelper::EventCallback(event);
          }
      }
  }
  );

  
  glfwSetMouseButtonCallback(GLHelper::ptr_window, [](GLFWwindow* window, int button, int action, int modes) {
      static_cast<void>(window);
      static_cast<void>(modes);
      if (button == GLFW_MOUSE_BUTTON_RIGHT) {
          if (action == GLFW_PRESS)
              rightMouseButtonPressed = true;
          else if (action == GLFW_RELEASE) {
              rightMouseButtonPressed = false;
              firstMouse = true;
          }
      }
      });

  //glfwSetCursorPosCallback(GLHelper::ptr_window, GLHelper::mousepos_cb);
  glfwSetScrollCallback(GLHelper::ptr_window, [](GLFWwindow* window, double xOffset, double yOffset) {
      static_cast<void>(window);
      static_cast<void>(xOffset);
      fov -= (float)yOffset;
      if (fov < 1.0f)
          fov = 1.0f;
      if (fov > 45.0f)
          fov = 45.0f;
      });

  glfwSetCursorPosCallback(GLHelper::ptr_window, [](GLFWwindow* window, double xPos, double yPos) {
      static_cast<void>(window);
      static_cast<float>(xPos);
      static_cast<float>(yPos);
      if (rightMouseButtonPressed) {
          if (firstMouse) {
              lastX = static_cast<float>(xPos);
              lastY = static_cast<float>(yPos);
              firstMouse = false;
          }

          float xoffset = static_cast<float>(xPos) - lastX;
          float yoffset = lastY - static_cast<float>(yPos); // Reversed since y-coordinates go from bottom to top
          lastX = static_cast<float>(xPos);
          lastY = static_cast<float>(yPos);

          float sensitivity = 0.1f;
          xoffset *= sensitivity;
          yoffset *= sensitivity;

          yaw += xoffset;
          pitch += yoffset;

          // Clamp pitch to prevent flipping
          if (pitch > 89.0f)
              pitch = 89.0f;
          if (pitch < -89.0f)
              pitch = -89.0f;

          // Update camera front vector
          glm::vec3 front;
          front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
          front.y = sin(glm::radians(pitch));
          front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
          cameraFront = glm::normalize(front);
      }
      });


  // this is the default setting ...
  glfwSetInputMode(GLHelper::ptr_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  // Part 2: Initialize entry points to OpenGL functions and extensions
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    std::cerr << "Unable to initialize GLEW - error: "
      << glewGetErrorString(err) << " abort program" << std::endl;
    return false;
  }
  if (GLEW_VERSION_4_5) {
    std::cout << "Using glew version: " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Driver supports OpenGL 4.5\n" << std::endl;
  } else {
    std::cerr << "Driver doesn't support OpenGL 4.5 - abort program" << std::endl;
    return false;
  }

  return true;
}


/*  _________________________________________________________________________ */
/*! cleanup

@param none

@return none

For now, there are no resources allocated by the application program.
The only task is to have GLFW return resources back to the system and
gracefully terminate.
*/
void GLHelper::cleanup() {
  // Part 1
  glfwTerminate();
}

/*  _________________________________________________________________________*/
/*! key_cb

@param GLFWwindow*
Handle to window that is receiving event

@param int
the keyboard key that was pressed or released

@parm int
Platform-specific scancode of the key

@parm int
GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
action will be GLFW_KEY_UNKNOWN if GLFW lacks a key token for it,
for example E-mail and Play keys.

@parm int
bit-field describing which modifier keys (shift, alt, control)
were held down

@return none

This function is called when keyboard buttons are pressed.
When the ESC key is pressed, the close flag of the window is set.
*/
//void GLHelper::key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod) {
//  if (GLFW_PRESS == action) {
//#ifdef _DEBUG
//    std::cout << "Key pressed" << std::endl;
//#endif
//  } else if (GLFW_REPEAT == action) {
//#ifdef _DEBUG
//    std::cout << "Key repeatedly pressed" << std::endl;
//#endif
//  } else if (GLFW_RELEASE == action) {
//#ifdef _DEBUG
//    std::cout << "Key released" << std::endl;
//#endif
//  }
//
//  if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
//    glfwSetWindowShouldClose(pwin, GLFW_TRUE);
//  }
//}

/*  _________________________________________________________________________*/
/*! mousebutton_cb

@param GLFWwindow*
Handle to window that is receiving event

@param int
the mouse button that was pressed or released
GLFW_MOUSE_BUTTON_LEFT and GLFW_MOUSE_BUTTON_RIGHT specifying left and right
mouse buttons are most useful

@parm int
action is either GLFW_PRESS or GLFW_RELEASE

@parm int
bit-field describing which modifier keys (shift, alt, control)
were held down

@return none

This function is called when mouse buttons are pressed.
*/
//void GLHelper::mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod) {
//  switch (button) {
//  case GLFW_MOUSE_BUTTON_LEFT:
//#ifdef _DEBUG
//    std::cout << "Left mouse button ";
//#endif
//    break;
//  case GLFW_MOUSE_BUTTON_RIGHT:
//#ifdef _DEBUG
//    std::cout << "Right mouse button ";
//#endif
//    break;
//  }
//  switch (action) {
//  case GLFW_PRESS:
//#ifdef _DEBUG
//    std::cout << "pressed!!!" << std::endl;
//#endif
//    break;
//  case GLFW_RELEASE:
//#ifdef _DEBUG
//    std::cout << "released!!!" << std::endl;
//#endif
//    break;
//  }
//}

/*  _________________________________________________________________________*/
/*! mousepos_cb

@param GLFWwindow*
Handle to window that is receiving event

@param double
new cursor x-coordinate, relative to the left edge of the client area

@param double
new cursor y-coordinate, relative to the top edge of the client area

@return none

This functions receives the cursor position, measured in screen coordinates but
relative to the top-left corner of the window client area.
*/
//void GLHelper::mousepos_cb(GLFWwindow *pwin, double xpos, double ypos) {
//#ifdef _DEBUG
//  std::cout << "Mouse cursor position: (" << xpos << ", " << ypos << ")" << std::endl;
//#endif
//}

/*  _________________________________________________________________________*/
/*! mousescroll_cb

@param GLFWwindow*
Handle to window that is receiving event

@param double
Scroll offset along X-axis

@param double
Scroll offset along Y-axis

@return none

This function is called when the user scrolls, whether with a mouse wheel or
touchpad gesture. Although the function receives 2D scroll offsets, a simple
mouse scroll wheel, being vertical, provides offsets only along the Y-axis.
*/
//void GLHelper::mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset) {
//#ifdef _DEBUG
//  std::cout << "Mouse scroll wheel offset: ("
//    << xoffset << ", " << yoffset << ")" << std::endl;
//#endif
//}

/*  _________________________________________________________________________ */
/*! error_cb

@param int
GLFW error code

@parm char const*
Human-readable description of the code

@return none

The error callback receives a human-readable description of the error and
(when possible) its cause.
*/
void GLHelper::error_cb(int error, char const* description) {
#ifdef _DEBUG
  std::cerr << "GLFW error: " << description << std::endl;
#endif
}

/*  _________________________________________________________________________ */
/*! fbsize_cb

@param GLFWwindow*
Handle to window that is being resized

@parm int
Width in pixels of new window size

@parm int
Height in pixels of new window size

@return none

This function is called when the window is resized - it receives the new size
of the window in pixels.
*/
void GLHelper::fbsize_cb(GLFWwindow *ptr_win, int _width, int _height) {
#ifdef _DEBUG
  std::cout << "fbsize_cb getting called!!!" << std::endl;
#endif
  // use the entire framebuffer as drawing region
  glViewport(0, 0, _width, _height);
  // later, if working in 3D, we'll have to set the projection matrix here ...
}

/*  _________________________________________________________________________*/
/*! update_time

@param double
fps_calc_interval: the interval (in seconds) at which fps is to be
calculated

This function must be called once per game loop. It uses GLFW's time functions
to compute:
1. the interval in seconds between each frame
2. the frames per second every "fps_calc_interval" seconds
*/
void GLHelper::update_time(double fps_calc_interval) {
  // get elapsed time (in seconds) between previous and current frames
  static double prev_time = glfwGetTime();
  double curr_time = glfwGetTime();
  delta_time = curr_time - prev_time;
  prev_time = curr_time;

  // fps calculations
  static double count = 0.0; // number of game loop iterations
  static double start_time = glfwGetTime();
  // get elapsed time since very beginning (in seconds) ...
  double elapsed_time = curr_time - start_time;

  ++count;

  // update fps at least every 10 seconds ...
  fps_calc_interval = (fps_calc_interval < 0.0) ? 0.0 : fps_calc_interval;
  fps_calc_interval = (fps_calc_interval > 10.0) ? 10.0 : fps_calc_interval;
  if (elapsed_time > fps_calc_interval) {
    GLHelper::fps = count / elapsed_time;
    start_time = curr_time;
    count = 0.0;
  }
}


/*  ________________________________________________________________________ */
/*! print_specs

@param none

@return none

This function is responsible for retrieving and printing various OpenGL
specifications such as vendor, renderer, version, shader version, major and
minor version, double buffering status, maximum vertex count, maximum indices
count, maximum texture size, and maximum viewport dimensions. It uses OpenGL
functions like glGetString and glGetIntegerv to query the specifications and
then prints them to the standard output.
*/
void GLHelper::print_specs() 
{
    GLubyte const* str_len = glGetString(GL_VENDOR);  
    std::cout << "GPU Vendor: " << str_len << std::endl;

    GLubyte const* str_renderer = glGetString(GL_RENDERER);
    std::cout << "GL Renderer: " << str_renderer << std::endl;

    GLubyte const* str_version = glGetString(GL_VERSION);
    std::cout << "GL Version: " << str_version << std::endl;

    GLubyte const* str_shader_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::cout << "GL Shader Version: " << str_shader_version << std::endl;

    GLint major_version;
    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    std::cout << "GL Major Version: " << major_version << std::endl;

    GLint minor_version;
    glGetIntegerv(GL_MINOR_VERSION, &minor_version);
    std::cout << "GL Minor Version: " << minor_version << std::endl;

    GLint double_buffer;
    glGetIntegerv(GL_DOUBLEBUFFER, &double_buffer);
    std::cout << "Current OpenGL Context is ";
    if (double_buffer) {
        std::cout << "double buffered";
    }
    else {
        std::cout << "single buffered";
    }
    std::cout << std::endl;

    GLint vertex_count;
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &vertex_count);
    std::cout << "Maximum Vertex Count: " << vertex_count << std::endl;


    GLint indices_count;
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &indices_count);
    std::cout << "Maximum Indices Count: " << indices_count << std::endl;

    GLint texutre_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texutre_size);
    std::cout << "GL Maximum texture size: " << texutre_size << std::endl;

    GLint max_viewport[2];
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, max_viewport);
    std::cout << "Maximum Viewport Dimensions: " << max_viewport[0] << " x " << max_viewport[1] << std::endl;

    GLint maxTextureUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    std::cout << "Maximum number of texture units: " << maxTextureUnits << std::endl;
}


//bool GLHelper::OnWindowClose(WindowCloseEvent& event) {
//    std::cout << "OnWindowsCloseEvent: Closing Window" << std::endl;
//    std::exit(EXIT_FAILURE);
//    return false;
//}


//void destroyInput() {
//    delete Input::GetInstance();
//    Input::InitializeInstance(nullptr);
//}
//bool GLHelper::WindowsInput::IsKeyPressedImpl(int keycode) {
//   auto state = glfwGetKey(ptr_window, keycode);
//   return state == GLFW_PRESS || state == GLFW_REPEAT;
//}

