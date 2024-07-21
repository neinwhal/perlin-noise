/*!************************************************************************
\file		my-tutorial-5.vert
\author		pghali@digipen.edu
\co-author  Bjorn Pokin Chinnaphongse
\param		bjornpokin.c@digipen.edu
\param		Course: CSD 2101
\param		Tutorial 5
\param		10/06/2023

\brief
This vertex shader source file is written in GLSL version 450 core and serves as the first part of the rendering pipeline, taking the input vertices and producing per-vertex output data.

The vertex shader receives vertex attributes through input variables:

- layout (location = 0) in vec2 aVertexPosition;
    - The position of the vertex. This is a 2D vector as input from attribute location 0.
- layout (location = 1) in vec3 aVertexColor;
    - The color of the vertex. This is a 3D vector as input from attribute location 1.
- layout (location = 2) in vec2 aTexCoord;
    - The texture coordinate of the vertex. This is a 2D vector as input from attribute location 2.

The shader also defines output variables:

- layout (location = 0) out vec3 vColor;
    - The output color of the vertex. This is a 3D vector passed to the fragment shader.
- layout (location = 1) out vec2 vTextureCoord;
    - The output texture coordinate. This is a 2D vector passed to the fragment shader after being multiplied by the texture scale factor.

Uniform variables used:

- uniform float uTextureScale;
    - The scaling factor for the texture coordinate. It scales the texture coordinates before passing them to the fragment shader.

The main function of this shader performs the operations:

- The vertex position is directly passed into the gl_Position variable. A vec4 is constructed to accommodate the 2D position vector, filling the Z component with 0.0 and the W component with 1.0.
- The vertex color is directly passed into the vColor output variable.
- The texture coordinate is multiplied by the scaling factor and the result is passed into the vTextureCoord output variable.

Overall, this shader file processes vertex data, including positions, colors, and texture coordinates, and prepares them for further stages in the OpenGL pipeline.
**************************************************************************/
#version 450 core

layout (location = 0) in vec2 aVertexPosition;
layout (location = 1) in vec3 aVertexColor;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec3 vColor;
layout (location = 1) out vec2 vTexureCoord; 

uniform float uTextureScale;
void main(void) {
   gl_Position = vec4(aVertexPosition, 0.0, 1.0);   
   vColor = aVertexColor;

   //TODO: out the vTexureCoord
   vTexureCoord = aTexCoord * uTextureScale;
}

