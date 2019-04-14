#pragma once

// Std
#include <vector>

// GL 
#define GLEW_STATIC
#include <GL/glew.h>

//GLM
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"

#include "AnimModel.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

struct leslie {
	//leslie position
	float lezX;
	float lezY;
	float lezZ;
	float lezRotation;
	bool isAnimated;
};

// Default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f; // up and down
const GLfloat SPEED = 6.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVTY), zoom(ZOOM)
	{
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();

		lez =
		{
			//leslie position
			0.0,
			-0.5,
			0.0,
			-180.0f,
			false
		};
	}

	// Constructor with scalar values
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVTY), zoom(ZOOM)
	{
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();

		lez =
		{
			//leslie position
			14.0f,
			-.05f,
			6.2f,
			-180.0f
		};
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}

	glm::vec3 getFrontDirection()
	{
		return frontDirection;
	}

	GLfloat getPitch()
	{
		return pitch;
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard( Camera_Movement direction, GLfloat deltaTime)
	{

		GLfloat velocity = this->movementSpeed * deltaTime;


		if (direction == FORWARD)
		{
			this->position += frontDirection * velocity;
			lez.lezX += frontDirection.x  * velocity;
			lez.lezZ += frontDirection.z  * velocity;

		}

		if (direction == BACKWARD)
		{
			this->position -= frontDirection * velocity;
			lez.lezX -= frontDirection.x  * velocity;
			lez.lezZ -= frontDirection.z  * velocity;
		}

		if (direction == LEFT)
		{
			this->position -= rightDirection * velocity;
			lez.lezX -= rightDirection.x  * velocity;
			lez.lezZ -= rightDirection.z  * velocity;
		}

		if (direction == RIGHT)
		{
			this->position += rightDirection * velocity;
			lez.lezX += rightDirection.x  * velocity;
			lez.lezZ += rightDirection.z  * velocity;
		}

	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true)
	{

		xOffset *= this->mouseSensitivity;
		yOffset *= this->mouseSensitivity;

		this->yaw += xOffset;
		this->pitch += yOffset;


		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (this->pitch > 89.0f)
			{
				this->pitch = 89.0f;
			}

			if (this->pitch < -89.0f)
			{
				this->pitch = -89.0f;
			}
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		this->updateCameraVectors();

	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yOffset)
	{

	}

	GLfloat GetZoom()
	{
		return this->zoom;
	}

	glm::vec3 GetPosition()
	{
		return this->position;
	}

	glm::vec3 GetFront()
	{
		return this->front;
	}

	GLfloat GetYaw()
	{
		return this->yaw;
	}

	glm::vec3 getRight()
	{
		return this->right;
	}

	glm::vec3 getUp()
	{
		return this->up;
	}

	//leslie
	leslie lez;

private:
	// Camera Attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 frontDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 rightDirection = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 worldUp;

	// Eular Angles
	GLfloat yaw;
	GLfloat pitch;

	// Camera options
	GLfloat movementSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoom;

	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		front.y = sin(glm::radians(this->pitch));
		front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->right = glm::normalize(glm::cross(this->front, this->worldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->up = glm::normalize(glm::cross(this->right, this->front));


		// rotate camera around the head radius
		this->position.x = lez.lezX + glm::cos(glm::radians(yaw)) *0.25f;
		this->position.z = lez.lezZ + glm::sin(glm::radians(yaw)) *0.25f;


		//update movement vector restricting the y axis
		glm::vec3 frontDir;
		frontDir.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		//frontDir.y = sin(glm::radians(this->pitch));
		front.y = frontDirection.y;
		frontDir.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->frontDirection = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->rightDirection = glm::normalize(glm::cross(this->front, this->worldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		
	}
};