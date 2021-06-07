#pragma once

#include "Material.h"
#include "Texture.h"
#include "Camera.h"

class Model {
	public:
		Model(const char* fileName, GLuint shader, glm::vec3 pos);
		~Model();
		void Render(Camera& camera);
		Material* material;
		Texture* texture;
		glm::vec3 position;

	private:
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		GLuint shader_, vao_;
		GLuint buffers[3];
		glm::mat4 model_;
};