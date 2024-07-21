/*!************************************************************************
\file		my-tutorial-5.frag
\author		pghali@digipen.edu
\co-author  Bjorn Pokin Chinnaphongse
\param		bjornpokin.c@digipen.edu
\param		Course: CSD 2101
\param		Tutorial 5
\param		10/06/2023

\brief
This fragment shader source file, written in GLSL version 450 core, is the part of the rendering pipeline that processes per-vertex data, interpolated over the primitive to be rendered, and computes the final color for each pixel in the frame buffer.

The fragment shader receives the following inputs:
- layout(location=0) in vec3 vColor;
    - The input color of the fragment. This is a 3D vector that was output from the vertex shader.
- layout (location=1) in vec2 vTextureCoord;
    - The input texture coordinate. This is a 2D vector output from the vertex shader after being scaled.

The shader also defines an output variable:
- layout (location=0) out vec4 fFragColor;
    - The final color of the fragment, a 4D vector that gets written into the frame buffer.

Uniform variables used:
- uniform int uTask;
    - The task number. Different values correspond to different rendering tasks.
- uniform int uModulating;
    - A binary flag that toggles modulation, which blends vertex color and texture color.
- uniform float uTileSize;
    - The size of each tile for the checkerboard pattern in task 1 and 2.
- uniform sampler2D uTex2d;
    - The 2D texture sampler used in task 3, 4, 5 and 6.

In the main function, it executes conditional branches based on the value of uTask. Each branch corresponds to a different rendering task:

- Task 0:
    - Passes the vertex color directly to the output color.
- Task 1 & 2:
    - Generates a checkerboard pattern using the screen-space coordinates. 
    - If modulating is off, the color of the fragment is either color0 or color1 based on the pattern. 
    - If modulating is on, the color is modulated by the vertex color.
- Task 3, 4, 5 & 6:
    - Maps a 2D texture onto the fragment. 
    - If modulating is on, the texture color is modulated by the vertex color.

Overall, this shader file takes in the color and texture coordinates from the vertex shader, processes them according to the task and modulation settings, and outputs the final color to the frame buffer.
**************************************************************************/

#version 450 core
layout (location=0) in vec3 vColor;
layout (location=1) in vec2 vTextureCoord;
layout (location=0) out vec4 fFragColor;

// task number
uniform int uTask;

// modulating 
uniform int uModulating;

// task 2 - tile size
uniform float uTileSize;

// task 3 - texture mapping
uniform sampler2D uTex2d;

//task 1
// function to calculate the color based on the tile position
int calculateColor(float tileXf, float tileYf) {
    int tileX = int(tileXf);
    int tileY = int(tileYf);
    return int(mod(float(tileX + tileY), 2.0));
}

// Task 1
float tileXf = gl_FragCoord.x / uTileSize;
float tileYf = gl_FragCoord.y / uTileSize;
int color = calculateColor(tileXf, tileYf);

vec3 color0 = vec3(1.0,0.0,1.0);
vec3 color1 = vec3(0.0,0.68, 0.94);

void main() {
	// task 0
	if(uTask == 0){
	 fFragColor = vec4(vColor, 1.0);  
	}

	// task 1
	else if (uTask == 1){
		// not modulating
		if(uModulating == 0){
			vec3 colorFinale = (color == 0) ? color0 : color1;
			fFragColor = vec4(colorFinale, 1.0);
		}
		// modulating
		else{
			vec3 colorFinale = (color == 0) ? color0 : color1;
			fFragColor = vec4(colorFinale * vColor, 1.0);
		}
	}

	// task 2
	else if(uTask == 2)
	{
		// not modulating
		if(uModulating == 0){
			vec3 colorFinale = (color == 0) ? color0 : color1;
			fFragColor = vec4(colorFinale, 1.0);
		}
		// modulating
		else{
			vec3 colorFinale = (color == 0) ? color0 : color1;
			fFragColor = vec4(colorFinale * vColor, 1.0);
		}
	}
	// task 3,4,5,6
	else if (uTask == 3 || uTask == 4 || uTask == 5 || uTask == 6){
	   vec4 textureColor = texture(uTex2d, vTextureCoord);

	   if(uModulating == 1){
		vec3 newColor = textureColor.rgb * vColor.rgb;
		fFragColor = vec4(newColor, textureColor.a);
	   }
	   else{
		fFragColor = textureColor;
	   }
	}
}
