#pragma once
#include <glm/gtc/matrix_transform.hpp> 


class Material {

	public:
		Material(float ns, glm::vec3 ka, glm::vec3 kd, glm::vec3 ks);

	private:
		float ns;
		glm::vec3 ka;
		glm::vec3 kd;
		glm::vec3 ks;

};