#include "Skybox.h"
#include <glm/gtc/type_ptr.hpp>

Skybox::Skybox(std::vector<std::string> cubeMap, GLuint shader) : 
	shader_ {shader} 
{
	texture = new Texture(cubeMap);
	
	// VAO
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	// VBO
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, 0);

	// Shader input
	GLint posId = glGetProgramResourceLocation(shader, GL_PROGRAM_INPUT, "vPosition");
	GLuint bindId = 0;
	glVertexAttribFormat(posId, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(posId, bindId);
	glBindVertexBuffer(bindId, buffer, 0, sizeof(GLfloat) * 3);
	glEnableVertexAttribArray(posId);

	// Set the texture
	GLuint skyboxId = glGetProgramResourceLocation(shader, GL_UNIFORM, "skybox");
	glProgramUniform1i(shader, skyboxId, texture->id);
}

void Skybox::Render(Camera &camera) {

	glBindVertexArray(vao_);
	glUseProgram(shader_);

	glm::mat4 mvp = camera.GetProjectionMatrix() * glm::mat4(glm::mat3(camera.GetViewMatrix()));
	GLuint mvpId = glGetProgramResourceLocation(shader_, GL_UNIFORM, "MVP");
	glProgramUniformMatrix4fv(shader_, mvpId, 1, GL_FALSE, glm::value_ptr(mvp));

	glDrawArrays(GL_TRIANGLES, 0, 36);
}

Skybox::~Skybox() {
	delete texture;
}