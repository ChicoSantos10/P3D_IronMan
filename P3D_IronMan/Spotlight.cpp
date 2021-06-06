#include "Spotlight.h"
#include <glm/gtc/type_ptr.hpp> 

Spotlight::Spotlight(glm::vec3 position, glm::vec3 direction, float kc, float kl, float kq, float cutoff, float exponent, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) 
	: PositionalLight(position, kc, kl, kq, ambient, diffuse, specular),
	direction_ {direction},
	cutoff_ {cutoff},
	exponent_ {exponent} 
{
	name_ = "spotLight";
}

void Spotlight::SetLightProperties(GLuint program) {
	GLuint posId = glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.position");
	GLuint ambId = glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.ambient");
	GLuint diffId = glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.diffuse");
	GLuint specId = glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.specular");
	GLuint dirId = glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.direction");
	GLuint kcId = glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.constant");
	GLuint klId = glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.linear");
	GLuint kqId = glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.quadratic");
	GLuint cutoffId = glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.cutoff");
	GLuint expId = glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.exponent");
	GLuint activeId = glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.isActive");

	glProgramUniform3fv(program, posId, 1, glm::value_ptr(position_));
	glProgramUniform3fv(program, ambId, 1, glm::value_ptr(ambient_));
	glProgramUniform3fv(program, diffId, 1, glm::value_ptr(diffuse_));
	glProgramUniform3fv(program, specId, 1, glm::value_ptr(specular_));
	glProgramUniform3fv(program, dirId, 1, glm::value_ptr(direction_));
	glProgramUniform1f(program, kcId, kc_);
	glProgramUniform1f(program, klId, kl_);
	glProgramUniform1f(program, kqId, kq_);
	glProgramUniform1f(program, cutoffId, cutoff_);
	glProgramUniform1f(program, expId, exponent_);
	glProgramUniform1ui(program, activeId, active_);
}
