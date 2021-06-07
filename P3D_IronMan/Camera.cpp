#include "Camera.h"
#include <iostream>

GLfloat rotation = 0;

Camera::Camera(glm::vec3 position, glm::vec3 &target, glm::vec3 offset, float aspectRatio) : 
	projection_{ glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f) },
	zoom_{ 10.0f },
	pos_{ position },
	target_{ &target },
	offset_ {offset}
{
	UpdateViewMatrix();
}

glm::mat4 Camera::GetProjectionMatrix() {
	return projection_;
}

glm::mat4 Camera::GetViewMatrix() {
	return view_;
}

void Camera::Zoom(double yOffset) {

	zoom_ += fabs(zoom_) * 0.1f * -(float)yOffset;

	UpdateViewMatrix();
}

void Camera::Rotate(GLfloat amount) {
	rotation += amount;
}

void Camera::Update() {
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix() {
	// Position of the camera changed so update view matrix
	glm::vec3 posAfterZoom = pos_ + glm::vec3(0, 0, zoom_);
	
	view_ = glm::lookAt(posAfterZoom, *target_ + offset_, glm::vec3(0.0, 1.0, 0.0));
	view_ = glm::rotate(view_, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
	
	/*using namespace std;
	cout << posAfterZoom.x << " " << posAfterZoom.y << " " << posAfterZoom.z << endl;
	cout << direction.x << " " << direction.y << " " << direction.z << endl;
	cout << "Target\nX: " << target_->x;
	cout << " Y: " << target_->y;
	cout << " Z: " << target_->z << endl;*/
}
