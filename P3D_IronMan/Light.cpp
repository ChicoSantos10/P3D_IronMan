#include "Light.h"

Light::Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
	ambient_ {ambient},
	diffuse_ {diffuse},
	specular_ {specular} {}

void Light::FlipSwitch(GLuint program) {

	active_ = !active_;

	std::string name = name_ + ".isActive";
	GLuint activeId = glGetProgramResourceLocation(program, GL_UNIFORM, name.c_str());

	glProgramUniform1ui(program, activeId, active_);
}