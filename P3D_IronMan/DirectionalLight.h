#pragma once

#include "Light.h"

class DirectionalLight : public Light {
public:
	DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void SetLightProperties(GLuint program);
protected:
	glm::vec3 direction_;
};
