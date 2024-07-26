/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
Course:     CS350
Project:    Assignment 1
Filename:   shader.hpp
Author(s):  Ang Wei Jie <weijie.ang@digipen.edu>
Date:       16/06/2024

Contents:
	- shader program handling
====================================================================================*/

#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#pragma warning(push)
#pragma warning(disable : 4201) // suppress glm/detail/type_quat.hpp warning: Warning C4201 nonstandard extension used : nameless struct / union
#include <glm/glm.hpp> 
#pragma warning(pop)

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

class shader {
	public:
		bool compile(GLenum shader_type, std::string file_name); // compile shader program
		bool link(); // link shader program
		bool validate(); // validate shader program
		bool compile_link_validate(std::vector<std::pair<GLenum, std::string>> vec); // combine above
		void use(); // use shader program
		void unuse(); // unuse shader program

		int get_handle() const { return program_handle; } // get shader program handle
		bool is_linked() const { return linked; } // check if shader program is linked

		enum shader_type { // shader types
			VERTEX_SHADER = GL_VERTEX_SHADER,
			FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
			GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
			TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER,
			TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER,
			COMPUTE_SHADER = GL_COMPUTE_SHADER
		};

	private:
		int program_handle{ 0 }; // shader program handle
		bool linked{ false }; // whether shader is linked
};

// container for shader program(s)
extern std::map<std::string, shader> shdrpgms;
void add_shader_programs(std::string shdr_pgm_name, std::string vtx_shdr_name, std::string frg_shdr_name);


#endif /* SHADER_HPP */