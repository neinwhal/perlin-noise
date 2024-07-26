#version 450 core
layout (location = 0) in vec3 aVertexPosition;
layout (location = 1) in vec3 aVertexColor;

uniform mat4 mvpMatrix;
out vec3 FragPos;
out vec3 Color;

void main(void) {
    gl_Position = mvpMatrix * vec4(aVertexPosition, 1.0);
    FragPos = aVertexPosition;
    Color = aVertexColor;
}