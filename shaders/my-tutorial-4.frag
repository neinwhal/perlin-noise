/*!************************************************************************
\file		my-tutorial-4.frag
\author		pghali@digipen.edu
\co-author  Bjorn Pokin Chinnaphongse
\param		bjornpokin.c@digipen.edu
\param		Course: CSD 2101
\param		Tutorial 4
\param		29/05/2023

\brief
This GLSL shader source file contains the fragment shader code for an OpenGL application. The shader primarily deals with the color attributes of the fragments.

The shader program performs the following operations:
	- Receives uniform color attribute supplied by the application.
	- Sets the output color of the fragment shader.

The functions and statements include:

#version 450 core
	- Specifies the version of GLSL to be used.

uniform vec3 uColor
	- Defines a uniform variable that receives the color attribute for every fragment.

layout (location=0) out vec4 fFragColor
	- Specifies the output color of the fragment shader.

main()
	- The entry point of the shader program. It sets the output color of the fragment shader by assigning the color uniform to fFragColor.
	- The shader ensures that every fragment of the triangle primitive rendered by an object has the same color, as dictated by the uniform color variable supplied by the application.
**************************************************************************/

#version 450 core
/*
A per-fragment color attribute is no longer received from rasterizer.
Instead per-fragment color attribute is supplied by the application to
a uniform variable:
uniform vec3 uColor;
The uniform variable will retain the value for every invocation of the
fragment shader. That is why every fragment of the triangle primitive
rendered by an object has the same color
*/
//layout(location=0) in vec3 vColor;

uniform vec3 uColor;
layout (location=0) out vec4 fFragColor;

void main() {
	//fFragColor = vec4(vColor, 1.0);
	  fFragColor = vec4(uColor, 1.0);
}
