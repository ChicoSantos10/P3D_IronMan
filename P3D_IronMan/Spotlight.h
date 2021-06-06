#pragma once

#include "PositionalLight.h"

class Spotlight : public PositionalLight
{
public:
	Spotlight(glm::vec3 position, glm::vec3 direction, float kc, float kl, float kq, float cutoff, float exponent, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void SetLightProperties(GLuint program) override;

protected:
	glm::vec3 direction_;
	float cutoff_, exponent_;
};
