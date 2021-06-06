#pragma once

#include "Light.h"

class AmbientLight : public Light
{
public:
	AmbientLight(glm::vec3 ambient);
	void SetLightProperties(GLuint program) override;
};
