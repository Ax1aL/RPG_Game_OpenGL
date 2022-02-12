#include "camera.h"

Camera::Camera(glm::vec3 cameraPosition) :
	cameraPosition(cameraPosition),
	worldUp(glm::vec3(0.f, 1.f, 0.f)),
	yaw(-90.f),
	pitch(0.f),
	speed(30.f),
	zoom(45.f),
	gravity(-1.f),
	cameraViewDirection(glm::vec3(0.f,0.f,-1.f))
{
	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(cameraPosition, cameraPosition + cameraViewDirection, cameraUp);
}

glm::vec3 Camera::getCameraPosition()
{
	return cameraPosition;
}

glm::vec3 Camera::getCameraViewDirection()
{
	return cameraViewDirection;
}

glm::vec3 Camera::getCameraUp()
{
	return cameraUp;
}

glm::vec3 Camera::getCameraRight()
{
	return cameraRight;
}

void Camera::updateCameraDirection(double dx, double dy)
{
	yaw += dx;
	pitch += dy;
	if (pitch > 89.f)pitch = 89.f;
	else if (pitch < -89.f)pitch = -89.f;
	updateCameraVectors();

}

void Camera::updateCameraPos(CameraDirection direction, double dt)
{
	

	float velocity = speed * (float)dt;

	switch (direction) {
	case CameraDirection::FRONT:
		cameraPosition +=glm::normalize(glm::vec3(cameraViewDirection.x,0.f, cameraViewDirection.z)) * velocity;break;//normalize and use only x and z glm::normalize(glm::vec3(cameraViewDirection.x,0.f,cameraViewDirection.z))
	case CameraDirection::BACK:
		cameraPosition -= glm::normalize(glm::vec3(cameraViewDirection.x, 0.f, cameraViewDirection.z)) * velocity; break;
	case CameraDirection::RIGHT:
		cameraPosition += cameraRight * velocity; break;
	case CameraDirection::LEFT:
		cameraPosition -= cameraRight * velocity; break;
	}
	
}

void Camera::updateCameraZoom(double dy)
{
	if (zoom >= 1.f && zoom <= 45.f) {
		zoom -= dy;
	}else if (zoom < 1.f) {
		zoom = 1.f;
	}
	else {//>45.0f
		zoom = 45.0f;
	}
}

float Camera::getYaw()
{
	return yaw;
}

float Camera::getPitch()
{
	return pitch;
}

void Camera::updateCameraVectors()
{
	
	cameraViewDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraViewDirection.y = sin(glm::radians(pitch));
	cameraViewDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraViewDirection = glm::normalize(cameraViewDirection);

	cameraRight = glm::normalize(glm::cross(cameraViewDirection, worldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraViewDirection));
}


