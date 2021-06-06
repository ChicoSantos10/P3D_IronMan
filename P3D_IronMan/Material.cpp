#include "Material.h"
#include <glm/gtc/type_ptr.hpp> 

Material::Material(float ns, glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, glm::vec3 emissive) :
	ns_ { ns },
	ka_ { ka },
	kd_ { kd },
	ks_ { ks },
	emissive_ { emissive } {}

void Material::SetShaderMaterial(GLuint program) {
	GLuint nsId = glGetProgramResourceLocation(program, GL_UNIFORM, "material.shininess");
	GLuint kaId = glGetProgramResourceLocation(program, GL_UNIFORM, "material.ambient");
	GLuint kdId = glGetProgramResourceLocation(program, GL_UNIFORM, "material.diffuse");
	GLuint ksId = glGetProgramResourceLocation(program, GL_UNIFORM, "material.specular");
	GLuint emissiveId = glGetProgramResourceLocation(program, GL_UNIFORM, "material.emissive");

	glProgramUniform1f(program, nsId, ns_);
	glProgramUniform3fv(program, kaId, 1, glm::value_ptr(ka_));
	glProgramUniform3fv(program, kdId, 1, glm::value_ptr(kd_));
	glProgramUniform3fv(program, ksId, 1, glm::value_ptr(ks_));
	glProgramUniform3fv(program, emissiveId, 1, glm::value_ptr(emissive_));
}