#pragma once

#include "Light.h"

class PositionalLight : public Light
{
public:
	PositionalLight(glm::vec3 position, float kc, float kl, float kq, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void SetLightProperties(GLuint program) override;

protected:
	glm::vec3 position_;
	float kc_, kl_, kq_;
};
