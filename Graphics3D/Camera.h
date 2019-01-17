#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class AbstractCamera
{
public:
	glm::vec3 Position;
	float Zoom = 45.0f;

	virtual glm::mat4 GetViewMatrix() = 0;

	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 60.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 60.0f)
			Zoom = 60.0f;
	}
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera: public AbstractCamera
{
public:
	// Camera Attributes
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		Zoom = ZOOM;
		updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		Zoom = ZOOM;
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	void SetYawPitch(float yaw, float pitch)
	{
		Yaw = yaw;

		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
		Pitch = pitch;

		updateCameraVectors();
	}
private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

class CarCamera: public AbstractCamera
{
public:
	// Camera Attributes
	//const float distance = 1.2f;
	//const float height = 0.4f;
	const float distance = 1.0f;
	const float height = 0.3f;

	glm::vec3 carPosition;
	const glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// Constructor with vectors
	CarCamera()
	{
		Position = glm::vec3(0.0f, 0.0f, -3.0f);
		carPosition = glm::vec3(0.0f, 0.0f, -2.0f);
	}

	void SetCarPosition(glm::vec3 carPos, glm::mat4 carModelMatrix)
	{
		carPosition = carPos;
		glm::vec4 distanceVector = glm::vec4(0.0f, height, distance, 1.0f);

		Position = glm::vec3(carModelMatrix * distanceVector);
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, carPosition, WorldUp);
	}
};

class StaticCamera : public AbstractCamera
{
public:
	// Camera Attributes
	const glm::vec3 CameraUp = glm::vec3(0.0f, 0.0f, 1.0f);

	// Constructor with vectors
	StaticCamera(glm::vec3 position)
	{
		Position = position;
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, glm::vec3(0.0f, 0.0f, 0.0f), CameraUp);
	}
};

class StaticFollowCamera : public AbstractCamera
{
public:
	// Camera Attributes

	glm::vec3 carPosition;
	const glm::vec3 CameraUp = glm::vec3(0.0f, 0.0f, 1.0f);

	// Constructor with vectors
	StaticFollowCamera(glm::vec3 position)
	{
		Position = position;
	}

	void SetCarPosition(glm::vec3 carPos, glm::mat4 carModelMatrix)
	{
		carPosition = carPos;
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, carPosition, CameraUp);
	}
};
#endif