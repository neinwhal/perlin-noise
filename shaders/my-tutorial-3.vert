/*!************************************************************************
\file		my-tutorial-3.vert
\author		pghali@digipen.edu
\co-author  Bjorn Pokin Chinnaphongse
\param		bjornpokin.c@digipen.edu
\param		Course: CSD 2101
\param		Tutorial 3
\param		23/05/2023

\brief
	This GLSL shader source file contains the vertex shader for an OpenGL application.
	This shader receives vertex positions and colors as inputs and passes the interpolated
	color to the fragment shader stage. It also applies a transformation from model to normalized
	device coordinates space (NDC).

	The shader program performs the following operations:
		- Receives vertex positions and colors from application.
		- Applies the transformation from model to NDC space.	
		- Sets the output color and position for the fragment shader.

	The functions include:
	- main()
		The entry point of the shader program. It applies the model to NDC transformation
		to the input vertex position and assigns it to gl_Position. It also sets the output
		color by assigning the input vertex color to vColor.
**************************************************************************/
#version 450 core

layout (location=0) in vec2 aVertexPosition;
layout (location=1) in vec3 aVertexColor;
layout (location=0) out vec3 vColor;

uniform mat3 uModel_to_NDC;

void main(void) {
	gl_Position = vec4(vec2(uModel_to_NDC * vec3(aVertexPosition, 1.f)),
                        0.0, 1.0);

	vColor = aVertexColor;
}
