/*!************************************************************************
\file		my-tutorial-4.vert
\author		pghali@digipen.edu
\co-author  Bjorn Pokin Chinnaphongse
\param		bjornpokin.c@digipen.edu
\param		Course: CSD 2101
\param		Tutorial 4
\param		29/05/2023

\brief
This GLSL shader source file contains the vertex shader code for an OpenGL application. The shader primarily deals with the position attributes of the vertices and the transformation of model to Normalized Device Coordinates (NDC).

The shader program performs the following operations:
	- Receives vertex positions as input.
	- Transforms model coordinates to NDC using a uniform transformation matrix.

The functions and statements include:

#version 450 core
	- Specifies the version of GLSL to be used.

layout (location=0) in vec2 aVertexPosition
	- Defines an input vertex attribute for the vertex positions.

uniform mat3 uModel_to_NDC
	- Defines a uniform transformation matrix to convert model coordinates to NDC.

main()
	- The entry point of the shader program. It calculates the transformed vertex positions and assigns them to gl_Position.
	- This vertex shader is a key component in rendering objects in an OpenGL application, by transforming vertex positions from model coordinates to NDC, enabling correct perspective and placement in the rendered scene.
**************************************************************************/
#version 450 core



//layout (location=1) in vec3 aVertexColor;
//layout (location=0) out vec3 vColor;

layout (location=0) in vec2 aVertexPosition;
uniform mat3 uModelToNDC;

void main(void) {
	gl_Position = vec4(vec2(uModelToNDC * vec3(aVertexPosition, 1.f)),
                        0.0, 1.0);

//	vColor = aVertexColor;
}
