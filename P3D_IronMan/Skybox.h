#pragma once

#include "Texture.h"
#include "Camera.h"

class Skybox
{
public:
	Skybox(std::vector<std::string> cubeMap, GLuint shader);
	~Skybox();
	Texture* texture;
	void Render(Camera &camera);

private:
	GLfloat vertices_[36 * 3] = {
		 1.0f, -1.0f, -1.0f,     1.0f, -1.0f,  1.0f,     1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,     1.0f,  1.0f, -1.0f,     1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,    -1.0f, -1.0f, -1.0f,    -1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,    -1.0f,  1.0f,  1.0f,    -1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,     1.0f,  1.0f, -1.0f,     1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,    -1.0f,  1.0f,  1.0f,    -1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,     -1.0f, -1.0f,  1.0f,    1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,     -1.0f, -1.0f,  1.0f,    1.0f, -1.0f,  1.0f,

		-1.0f, -1.0f,  1.0f,    -1.0f,  1.0f,  1.0f,     1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,     1.0f, -1.0f,  1.0f,    -1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,    -1.0f, -1.0f, -1.0f,     1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,     1.0f,  1.0f, -1.0f,    -1.0f,  1.0f, -1.0f
	};
	GLuint vao_;
	GLuint shader_;
};
