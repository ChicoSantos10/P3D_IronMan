#include <glm/gtc/matrix_transform.hpp> 
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "Model.h"
#include "FileReader.h"


Model::Model(const char* fileName, GLuint shader, glm::vec3 pos) :
	shader_ {shader},
	model_ {glm::translate(glm::mat4(1.0f), position)},
	position {position}
{
    std::string mtl;
    if (FileReader::OBJReader(fileName, positions, uvs, normals, mtl)) {
        
        float ns;
        glm::vec3 ka, kd, ks;
        std::string texturePath;

        if (FileReader::MTLReader(mtl.c_str(), ns, ka, kd, ks, texturePath)) {
            texture = new Texture(texturePath);
            material = new Material(ns, ka, kd, ks);
			material->SetShaderMaterial(shader_);
        }
    }

	// VAO
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	// VBOs
	glGenBuffers(3, buffers);

	// Shader
	glUseProgram(shader);

	// Position buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), 0);
	GLint posId = glGetProgramResourceLocation(shader, GL_PROGRAM_INPUT, "vpos");
	GLint bindId = 0;
	glVertexAttribFormat(posId, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(posId, bindId);
	glBindVertexBuffer(bindId, buffers[0], 0, sizeof(GLfloat) * 3);
	glEnableVertexAttribArray(posId);

	// Uv buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), uvs.data(), 0);
	GLint uvId = glGetProgramResourceLocation(shader, GL_PROGRAM_INPUT, "vtexcoord");
	bindId = 1;
	glVertexAttribFormat(uvId, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(uvId, bindId);
	glBindVertexBuffer(bindId, buffers[1], 0, sizeof(GLfloat) * 2);
	glEnableVertexAttribArray(uvId);

	// Normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), 0);
	GLint normalId = glGetProgramResourceLocation(shader, GL_PROGRAM_INPUT, "vnormal");
	bindId = 2;
	glVertexAttribFormat(normalId, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(normalId, bindId);
	glBindVertexBuffer(bindId, buffers[2], 0, sizeof(GLfloat) * 3);
	glEnableVertexAttribArray(normalId);

	// Textures
	GLint texId = glGetProgramResourceLocation(shader, GL_UNIFORM, "image");
	glProgramUniform1i(shader, texId, texture->id);
}

void Model::Render(Camera& camera) {
	// Projection
	GLint projectionId = glGetProgramResourceLocation(shader_, GL_UNIFORM, "projection");
	glProgramUniformMatrix4fv(shader_, projectionId, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix()));

	// MVP
	glm::mat4 mvp = camera.GetProjectionMatrix() * camera.GetViewMatrix() * model_;
	GLint mvpId = glGetProgramResourceLocation(shader_, GL_UNIFORM, "MVP");
	glProgramUniformMatrix4fv(shader_, mvpId, 1, GL_FALSE, glm::value_ptr(mvp));

	// Model
	GLint modelId = glGetProgramResourceLocation(shader_, GL_UNIFORM, "model");
	glProgramUniformMatrix4fv(shader_, modelId, 1, GL_FALSE, glm::value_ptr(model_));

	// View	
	GLint viewId = glGetProgramResourceLocation(shader_, GL_UNIFORM, "view");
	glProgramUniformMatrix4fv(shader_, viewId, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));

	// View Model
	GLint viewModelId = glGetProgramResourceLocation(shader_, GL_UNIFORM, "viewModel");
	glm::mat4 viewModel = camera.GetViewMatrix() * model_;
	glProgramUniformMatrix4fv(shader_, viewModelId, 1, GL_FALSE, glm::value_ptr(viewModel));

	// Normal Matrix
	GLint normalId = glGetProgramResourceLocation(shader_, GL_UNIFORM, "normalMatrix");
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(viewModel));
	glProgramUniformMatrix3fv(shader_, normalId, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	glBindVertexArray(vao_);
	glUseProgram(shader_);

	glDrawArrays(GL_TRIANGLES, 0, positions.size());
}

Model::~Model() {
    delete material;
    delete texture;
}