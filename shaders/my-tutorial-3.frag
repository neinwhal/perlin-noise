/*!************************************************************************
\file		my-tutorial-3.frag
\author		pghali@digipen.edu
\co-author  Bjorn Pokin Chinnaphongse
\param		bjornpokin.c@digipen.edu
\param		Course: CSD 2101
\param		Tutorial 3
\param		23/05/2023

\brief
This GLSL shader source file contains the fragment shader for an OpenGL application. 
This shader receives interpolated colors from the vertex shader and passes them to 
the fragment shader stage. The shader program performs the following operations:
  - Receives interpolated colors from vertex shader.
  - Sets the output color of the fragment shader.

The functions include:
  - main
    - The entry point of the shader program. It sets the output color of the fragment 
      shader by assigning the interpolated color to fFragColor.
**************************************************************************/

#version 450 core

layout (location=0) in vec3 vInterpColor;
layout (location=0) out vec4 fFragColor;

void main () {
	fFragColor = vec4(vInterpColor, 1.0);
}
