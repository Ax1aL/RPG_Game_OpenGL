#pragma once

#include <glm.hpp>
#include <gtx\transform.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include "..\Graphics\window.h"


enum class CameraDirection {
	FRONT=0,
	BACK,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
	public://should be private
		glm::vec3 cameraPosition;
		glm::vec3 cameraViewDirection;
		glm::vec3 cameraUp;
		glm::vec3 cameraRight;
		glm::vec3 worldUp;
		
		float yaw;
		float pitch;
		float speed;
		float zoom;

		float gravity;
		



	public:
		Camera(glm::vec3 cameraPosition);
		glm::mat4 getViewMatrix();
		glm::vec3 getCameraPosition();
		glm::vec3 getCameraViewDirection();
		glm::vec3 getCameraUp();
		glm::vec3 getCameraRight();
		void updateCameraDirection(double dx, double dy);
		void updateCameraPos(CameraDirection dir, double dt);
		void updateCameraZoom(double dy);
		float getYaw();
		float getPitch();

private:
	void updateCameraVectors();



};

