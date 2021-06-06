#pragma once

#include <glm/gtc/matrix_transform.hpp> 
#include <string>
#include <vector>

class FileReader {
	public:
		static bool OBJReader(const char* fileName, std::vector<glm::vec3>& positions, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::string& mtl);
		static bool MTLReader(const char* fileName, float& ns, glm::vec3& ka, glm::vec3& kd, glm::vec3& ks, std::string& texture);
		static void Error_OpenFile(const char* fileName);
};

