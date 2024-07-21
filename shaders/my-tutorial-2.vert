/*!************************************************************************
\file		my-tutorial-2.vert
\author		pghali@digipen.edu
\co-author  Bjorn Pokin Chinnaphongse
\param		bjornpokin.c@digipen.edu
\param		Course: CSD 2101
\param		Tutorial 2
\param		14/05/2023

\brief
This GLSL shader source file contains the vertex shader for an OpenGL application. 
This shader receives vertex positions and colors as inputs and passes them to 
the fragment shader stage. The shader program performs the following operations:
  - Receives vertex positions and colors from application.
  - Sets the output color and position for the fragment shader.

The functions include:
  - main
    - The entry point of the shader program. It sets the output position of the vertex 
      shader by assigning the input vertex position to gl_Position. It also sets the 
      output color by assigning the input vertex color to vColor.
**************************************************************************/
#version 450 core

layout (location=0) in vec2 aVertexPosition;
layout (location=1) in vec3 aVertexColor;

layout (location=0) out vec3 vColor;

void main(void) {
	gl_Position = vec4(aVertexPosition, 0.0, 1.0);
	vColor = aVertexColor;
}
