#pragma once

#include "Material.h"
#include "Texture.h"

class Model {
	public:
		Model(const char* fileName);
		~Model();
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		Material* material;
		Texture* texture;

	private:
		
};