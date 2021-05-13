#include "Camera.h"
#include <iostream>

using namespace openGL_program;

Camera::Camera(glm::vec3 position, glm::vec3 target, float aspectRatio) : 
	projection_{ glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f) },
	zoom_{ 10.0f },
	pos_{ position },
	target_{ target } 
{
	glm::vec3 posAfterZoom = pos_ + glm::vec3(0, 0, zoom_); // TODO: Make it so zoom moves forward based on the camera rotation
	glm::vec3 direction = target_ - pos_;
	view_ = glm::lookAt(posAfterZoom, posAfterZoom + direction, glm::vec3(0.0, 1.0, 0.0));
}

glm::mat4 Camera::GetProjectionMatrix() {
	return projection_;
}

glm::mat4 Camera::GetViewMatrix() {
	return view_;
}

void Camera::Zoom(double yOffset) {

	using namespace std;
	cout << "Zoom: " << zoom_ << endl;
	cout << "Offset: " << yOffset << endl;
	zoom_ += fabs(zoom_) * 0.1f * -(float)yOffset;
	cout << "New zoom: " << zoom_ << endl;

	// Position of the camera changed so update view matrix
	glm::vec3 posAfterZoom = pos_ + glm::vec3(0, 0, zoom_); // TODO: Make it so zoom moves forward based on the camera rotation
	glm::vec3 direction = target_ - pos_;
	view_ = glm::lookAt(posAfterZoom, posAfterZoom + direction, glm::vec3(0.0, 1.0, 0.0));
}