#include "PositionalLight.h"
#include <glm/gtc/type_ptr.hpp> 

PositionalLight::PositionalLight(glm::vec3 position, float kc, float kl, float kq, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) 
	: Light(ambient, diffuse, specular), 
	position_ {position},
	kc_ {kc},
	kl_ {kl},
	kq_ {kq} 
{
	name_ = "pointLight";
}

void PositionalLight::SetLightProperties(GLuint program) {
	GLuint posId = glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight.position");
	GLuint ambId = glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight.ambient");
	GLuint diffId = glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight.diffuse");
	GLuint specId = glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight.specular");
	GLuint kcId = glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight.constant");
	GLuint klId = glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight.linear");
	GLuint kqId = glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight.quadratic");
	GLuint activeId = glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight.isActive");

	glProgramUniform3fv(program, posId, 1, glm::value_ptr(position_));
	glProgramUniform3fv(program, ambId, 1, glm::value_ptr(ambient_));
	glProgramUniform3fv(program, diffId, 1, glm::value_ptr(diffuse_));
	glProgramUniform3fv(program, specId, 1, glm::value_ptr(specular_));
	glProgramUniform1f(program, kcId, kc_);
	glProgramUniform1f(program, klId, kl_);
	glProgramUniform1f(program, kqId, kq_);
	glProgramUniform1ui(program, activeId, active_);
}