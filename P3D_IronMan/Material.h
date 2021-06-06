#pragma once
#include <glm/gtc/matrix_transform.hpp> 
#include <GL\glew.h>

class Material {

	public:
		Material(float ns, glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, glm::vec3 emissive = glm::vec3(0.0f, 0.0f, 0.0f));
		void SetShaderMaterial(GLuint program);

	private:
		float ns_;
		glm::vec3 ka_;
		glm::vec3 kd_;
		glm::vec3 ks_;
		glm::vec3 emissive_;
};