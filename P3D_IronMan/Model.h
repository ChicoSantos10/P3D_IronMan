#pragma once

#include "Material.h"

class Model {
	public:
		Model(const char* fileName);
		~Model();
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		int width, height, channels;
		unsigned char* texture;
		Material* material;

	private:
		
};