#include "DirectionalLight.h"
#include <glm/gtc/type_ptr.hpp> 

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) 
	: Light(ambient, diffuse, specular), 
	direction_ {direction} 
{
	name_ = "directionalLight";
}

void DirectionalLight::SetLightProperties(GLuint program) {
	GLuint dirId = glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.direction");
	GLuint ambId = glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.ambient");
	GLuint diffId = glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.diffuse");
	GLuint specId = glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.specular");
	GLuint activeId = glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.isActive");

	glProgramUniform3fv(program, dirId, 1, glm::value_ptr(direction_));
	glProgramUniform3fv(program, ambId, 1, glm::value_ptr(ambient_));
	glProgramUniform3fv(program, diffId, 1, glm::value_ptr(diffuse_));
	glProgramUniform3fv(program, specId, 1, glm::value_ptr(specular_));
	glProgramUniform1ui(program, activeId, active_);
}