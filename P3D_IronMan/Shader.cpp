#pragma once

#include <iostream>
#include <fstream>

#define GLEW_STATIC
#include <GL\glew.h>

#include "Shader.h"

#define _DEBUG

static const GLchar* ReadShader(const char* filename) {
	
	std::ifstream file(filename, std::ifstream::ate | std::ifstream::binary);
	
	if (file.is_open()) {
		
		std::streampos sizeBytes = file.tellg();

		// Go back to the beggining
		file.seekg(0, std::ios::beg);

		// Memory allocation
		GLchar* source = new GLchar[int(sizeBytes) + 1];
		
		file.read(source, sizeBytes);
		
		// Close the string
		source[sizeBytes] = 0;

		file.close();

		return const_cast<const GLchar*>(source);
	}
	else {
		std::cerr << "Could not open the file: '" << filename << "'" << std::endl;
	}

	return nullptr;
}

GLuint LoadShaders(ShaderInfo* shaders) {
	if (shaders == nullptr) return 0;

	GLuint program = glCreateProgram();

	for (GLint i = 0; shaders[i].type != GL_NONE; i++) {
		
		shaders[i].shader = glCreateShader(shaders[i].type);

		
		const GLchar* source = ReadShader(shaders[i].filename);
		
		// Couldn't read the file so delete all shaders
		if (source == NULL) {
			
			for (int j = 0; shaders[j].type != GL_NONE; j++) {
				
				if (shaders[j].shader != 0)
					glDeleteShader(shaders[j].shader);
				shaders[j].shader = 0;
			}

			return 0;
		}

		// Load the shader
		glShaderSource(shaders[i].shader, 1, &source, NULL);
		delete[] source;

		// Tries to compile
		glCompileShader(shaders[i].shader);

		// Check if it compiled
		GLint compiled;
		glGetShaderiv(shaders[i].shader, GL_COMPILE_STATUS, &compiled);

		// Failed to compile. Delete shaders
		if (!compiled) {
#ifdef _DEBUG
			GLsizei len;
			glGetShaderiv(shaders[i].shader, GL_INFO_LOG_LENGTH, &len);

			GLchar* log = new GLchar[len + 1];
			glGetShaderInfoLog(shaders[i].shader, len, &len, log);
			std::cerr << "Shader compilation failed: " << log << std::endl;
			delete[] log;
#endif /* DEBUG */

			for (int j = 0; shaders[j].type != GL_NONE; j++) {
				
				if (shaders[j].shader != 0)
					glDeleteShader(shaders[j].shader);
				shaders[j].shader = 0;
			}

			return 0;
		}

		glAttachShader(program, shaders[i].shader);
	}

	glLinkProgram(program);

	// Check if it linked successfully
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	
	// Failed to link. Delete shaders
	if (!linked) {
#ifdef _DEBUG
		GLsizei len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len + 1];
		glGetProgramInfoLog(program, len, &len, log);
		std::cerr << "Shader linking failed: " << log << std::endl;
		delete[] log;
#endif /* DEBUG */

		for (int j = 0; shaders[j].type != GL_NONE; j++) {
			
			if (shaders[j].shader != 0)
				glDeleteShader(shaders[j].shader);
			shaders[j].shader = 0;
		}

		return 0;
	}

	return program;
}