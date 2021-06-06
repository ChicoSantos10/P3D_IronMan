#include "AmbientLight.h"
#include <glm/gtc/type_ptr.hpp> 
#include <iostream>

AmbientLight::AmbientLight(glm::vec3 ambient) : Light(ambient, glm::vec3(0,0,0), glm::vec3(0,0,0))
{
	name_ = "ambientLight";
}

void AmbientLight::SetLightProperties(GLuint program) {
	GLuint ambId = glGetProgramResourceLocation(program, GL_UNIFORM, "ambientLight.ambient");
	GLuint activeId = glGetProgramResourceLocation(program, GL_UNIFORM, "ambientLight.isActive");

	glProgramUniform3fv(program, ambId, 1, glm::value_ptr(ambient_));
	glProgramUniform1ui(program, activeId, active_);
} 