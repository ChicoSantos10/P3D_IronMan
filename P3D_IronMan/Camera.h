#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

class Camera {
	public:
		Camera(glm::vec3 position, glm::vec3 &target, glm::vec3 offset, float aspectRatio);
		glm::mat4 GetViewMatrix();
		glm::mat4 GetProjectionMatrix();
		void Zoom(double yOffset);
		void Rotate(GLfloat amount);
		void Update();
	private:
		void UpdateViewMatrix();
		glm::mat4 view_;
		glm::mat4 projection_;
		glm::vec3 pos_;
		glm::vec3 *target_;
		glm::vec3 offset_;
		GLfloat zoom_;
};