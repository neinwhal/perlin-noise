/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
Course:     CS350
Project:    Assignment 1
Filename:   shader.cpp
Author(s):  Ang Wei Jie <weijie.ang@digipen.edu>
Date:       16/06/2024

Contents:
	- shader program handling
====================================================================================*/


#include "shader.hpp"
#include <iostream>

// list of shaders
std::map<std::string, shader> shdrpgms;

/*!*****************************************************************************
\brief
	Helper to add shader programs for compiling, linking and validating, store
	shader to shader storage container upon success, exits upon failure

\param[in] shader_program_name
	string as shader program name (identifier)

\param[in] vertex_shader_path
	string as path to vertex shader

\param[in] fragment_shader_path
	string as path to fragment shader

\return
	none, shader automatically added to shader storage container upon success
*******************************************************************************/
void add_shader_programs(std::string shader_program_name, std::string vertex_shader_path, std::string fragment_shader_path) {
	std::vector<std::pair<GLenum, std::string>> shdr_files{
		std::make_pair(GL_VERTEX_SHADER, vertex_shader_path),
		std::make_pair(GL_FRAGMENT_SHADER, fragment_shader_path)
	};
	shader shdr_pgm;
	shdr_pgm.compile_link_validate(shdr_files); // compile, link and validate shader program
	if (shdr_pgm.is_linked() == false) {
		std::cout << "[shader] Unable to compile/link/validate shader programs" << std::endl; // error if unable to build
	}
	// add compiled, linked, and validated shader program to std::map container shdrpgms
	shdrpgms[shader_program_name] = shdr_pgm;
}

/*!*****************************************************************************
\brief
	Checks whether file exists

\param[in] file_name
	string of path to file to check

\return
	whether file exists
*******************************************************************************/
bool file_exists(std::string const& file_name) {
	std::ifstream file_stream(file_name);
	return file_stream.good();
}

/*!*****************************************************************************
\brief
	Compiles shader program

\param[in] shader_type
	GLenum of shader type

\param[in] file_name
	string of path to shader file

\return
	whether shader compilation is successful
	exits and gives a crash log upon failure
*******************************************************************************/
bool shader::compile(GLenum shader_type, std::string file_name) {
	// check if file exists
	if (!file_exists(file_name)) {
		std::cout << "[shader] shader file " << file_name.c_str() << " not found!" << std::endl;
		return false;
	}

	// request a new program handle
	if (program_handle <= 0) {
		program_handle = glCreateProgram();
		if (program_handle == 0) {
			std::cout << "[shader] cannot create program handle for shader!" << std::endl;
			return false;
		}
	}

	// open shader file
	std::ifstream shader_file(file_name, std::ifstream::in);
	if (!shader_file) {
		std::cout << "[shader] unable to open shader file: " << file_name.c_str() << std::endl;
		return false;
	}

	// read shader file and store it
	std::stringstream buffer;
	buffer << shader_file.rdbuf();
	shader_file.close();
	std::string shader_source{ buffer.str() };

	// determine handle depending on shader type
	int shader_handle{ 0 };
	switch (shader_type) {
	case VERTEX_SHADER: shader_handle = glCreateShader(GL_VERTEX_SHADER); break;
	case FRAGMENT_SHADER: shader_handle = glCreateShader(GL_FRAGMENT_SHADER); break;
	case GEOMETRY_SHADER: shader_handle = glCreateShader(GL_GEOMETRY_SHADER); break;
	case TESS_CONTROL_SHADER: shader_handle = glCreateShader(GL_TESS_CONTROL_SHADER); break;
	case TESS_EVALUATION_SHADER: shader_handle = glCreateShader(GL_TESS_EVALUATION_SHADER); break;
	case COMPUTE_SHADER: shader_handle = glCreateShader(GL_COMPUTE_SHADER); break;
	default:
		std::cout << "[shader] incorrect shader type supplied! : " << shader_type << std::endl;
		return GL_FALSE;
	}

	// load shader code into shader object
	char const* shader_code[] = { shader_source.c_str() };
	glShaderSource(shader_handle, 1, shader_code, nullptr);

	// compile shader
	glCompileShader(shader_handle);

	// compile status
	int compile_status;
	glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &compile_status);
	if (compile_status == false) {
		// error message handling
		std::string msg{ "[shader] vertex shader compilation failed! \n" };

		GLint log_length;
		glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_length);
		if (log_length > 0) {
			GLchar* error_log = new GLchar[log_length];
			GLsizei written_log_length;
			glGetShaderInfoLog(shader_handle, log_length, &written_log_length, error_log);
			msg += error_log;
			delete[] error_log;
		}
		std::cout << msg.c_str() << std::endl;
		return false;
	}
	else {
		// success! attach shader program
		glAttachShader(program_handle, shader_handle);
		return true;
	}
}

/*!*****************************************************************************
\brief
	Links shader program

\param
	none

\return
	whether shader linking is successful
	exits and gives a crash log upon failure
*******************************************************************************/
bool shader::link() {
	// check if already linked
	if (linked) {
		return true;
	}
	// check program handle is valid
	if (program_handle <= 0) {
		return false;
	}

	glLinkProgram(program_handle); // link compiled shaders

	// link status
	int link_status;
	glGetProgramiv(program_handle, GL_LINK_STATUS, &link_status);
	if (link_status == false) {
		// error message handling
		std::string msg{ "[shader] failed to link shader program! \n" };

		GLint log_length;
		glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &log_length);
		if (log_length > 0) {
			GLchar* error_log = new GLchar[log_length];
			GLsizei written_log_length;
			glGetProgramInfoLog(program_handle, log_length, &written_log_length, error_log);
			msg += error_log;
			delete[] error_log;
		}

		std::cout << msg.c_str() << std::endl;
		return false;
	}
	else {
		// success! set shader status to linked
		linked = true;
		return true;
	}
}

/*!*****************************************************************************
\brief
	Validates shader program

\param
	none

\return
	whether shader validation is successful
	exits and gives a crash log upon failure
*******************************************************************************/
bool shader::validate() {
	// ensure linked and program handle valid
	if (!linked || program_handle <= 0) {
		return false;
	}

	// validate shader program
	glValidateProgram(program_handle);

	// validate status
	int validate_status;
	glGetProgramiv(program_handle, GL_VALIDATE_STATUS, &validate_status);
	if (validate_status == false) {
		// error message handling
		std::string msg{ "[graphics] failed to validate shader program for current openGL context! \n" };

		GLint log_length;
		glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &log_length);
		if (log_length > 0) {
			GLchar* error_log = new GLchar[log_length];
			GLsizei written_log_length;
			glGetProgramInfoLog(program_handle, log_length, &written_log_length, error_log);
			msg += error_log;
			delete[] error_log;
		}
		std::cout << msg.c_str() << std::endl;
		return false;
	}
	else {
		// success!
		return true;
	}
}

/*!*****************************************************************************
\brief
	Compile, link and validates shader program together

\param[in] shader_list
	list of shaders (in pairs, shader type and path to shader file) to compile,
	link and validate

\return
	whether shader compilation, linking and validation is successful
*******************************************************************************/
bool shader::compile_link_validate(std::vector<std::pair<GLenum, std::string>> shader_list) {
	for (auto& element : shader_list) {
		// compile all programs
		if (!compile(element.first, element.second.c_str())) {
			return false;
		}
	}
	// link
	if (!link()) return false;
	// validate
	if (!validate()) return false;
	return true;
}

/*!*****************************************************************************
\brief
	Use shader program

\param
	none

\return
	none
*******************************************************************************/
void shader::use() {
	if (program_handle > 0 && linked) { // ensure shader is linked and program handle is valid
		glUseProgram(program_handle);
	}
}

/*!*****************************************************************************
\brief
	Unuse shader program

\param
	none

\return
	none
*******************************************************************************/
void shader::unuse() {
	glUseProgram(0);
}