#pragma once

class Model {
	public:
		Model(const char* fileName);

	private:
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
};