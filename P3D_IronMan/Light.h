#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

class Light {
public:
	Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	virtual void SetLightProperties(GLuint program) = 0;
	void FlipSwitch(GLuint program);
protected:
	glm::vec3 ambient_, diffuse_, specular_;
	std::string name_;
	bool active_ = true;
};
