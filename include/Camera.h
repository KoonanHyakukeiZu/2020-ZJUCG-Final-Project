#ifndef CAMERA_H
#define CAMERA_H

#define PI 3.14159265

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <math.h>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	NORTH,
	SOUTH,
	EAST,
	WEST,
	NONE
};

enum Camera_Scroll {
	FOVY_CHANGE,
	HEIGHT_CHANGE
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = -45.0f;
const float SPEED = 10.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float MIN_H = 1.0f;
const float MAX_H = 100.0f;



// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	glm::vec3 North = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 East = glm::vec3(1.0f, 0.0f, 0.0f);

	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	
	// Added two constructors 
	// constructor with the traditional "gluLookAt" style (vector)
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.1f, 1.0f), glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		Front = glm::normalize(center - position);
		WorldUp = up;
		Pitch = asin(Front.y) * 180.0 / PI;
		float tmp = Front.z / cos(Pitch / 180.0 * PI);
		if (tmp > 1)
			tmp = 0.999;
		if (tmp < -1)
			tmp = -0.999;
		Yaw = asin(tmp) * 180.0 / PI;
		updateCameraVectors();
	}
	// constructor with the traditional "gluLookAt" style (scalar)
	Camera(float posX, float posY, float posZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		Front = glm::normalize(glm::vec3(centerX, centerY, centerZ) - Position);
		WorldUp = glm::vec3(upX, upY, upZ);
		Pitch = asin(Front.y) * 180.0 / PI;
		float tmp = Front.z / cos(Pitch / 180.0 * PI);
		if (tmp > 1)
			tmp = 0.999;
		if (tmp < -1)
			tmp = -0.999;
		Yaw = asin(tmp) * 180.0 / PI;
		updateCameraVectors();
	}
	// constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	Camera(const Camera& right)
	{
		MovementSpeed = right.MovementSpeed;
		MouseSensitivity = right.MouseSensitivity;
		Zoom = right.Zoom;

		Position = right.Position;
		WorldUp = right.WorldUp;
		Yaw = right.Yaw;
		Pitch = right.Pitch;
		updateCameraVectors();
	}

	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		updateCameraVectors();
		return glm::lookAt(Position, Position + Front, Up);
	}

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(float deltaTime, Camera_Movement direction = NONE)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += glm::vec3(Front.x, 0.0f, Front.z) * velocity;
		else if (direction == BACKWARD)
			Position -= glm::vec3(Front.x, 0.0f, Front.z) * velocity;
		else if (direction == LEFT)
			Position -= Right * velocity;
		else if (direction == RIGHT)
			Position += Right * velocity;
		else if (direction == NORTH)
			Position += North * velocity;
		else if (direction == SOUTH)
			Position -= North * velocity;
		else if (direction == EAST)
			Position += East * velocity;
		else if (direction == WEST)
			Position -= East * velocity;
	}

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw   += xoffset;
		Pitch += yoffset;

		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	void ProcessMouseMovement(float xoffset, float yoffset, const glm::vec3& center)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw -= xoffset;
		Pitch -= yoffset;

		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;

		glm::vec3 d3 = Position - center;
		glm::vec4 d4 = glm::vec4(d3.x, d3.y, d3.z, 1.0f);
		glm::mat4 rot = glm::mat4(1.0f);
		rot = glm::rotate(rot, glm::radians(xoffset), glm::vec3(0.0f, 1.0f, 0.0f));
		rot = glm::rotate(rot, glm::radians(-yoffset), glm::vec3(1.0f, 0.0f, 0.0f));
		d4 = rot * d4;
		Position = center + glm::vec3(d4.x, d4.y, d4.z);

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis

	void ProcessMouseScroll(Camera_Scroll scrollAct, float yoffset)
	{
		if (scrollAct == HEIGHT_CHANGE)
		{
			glm::vec3 old = Position;
			Position += yoffset * Front;
			if (Position.y < MIN_H)
				Position = old;
			else if (Position.y > MAX_H)
				Position = old;
		}
		else if (scrollAct == FOVY_CHANGE)
		{
			Zoom -= (float)yoffset;
			if (Zoom < 1.0f)
				Zoom = 1.0f;
			if (Zoom > 45.0f)
				Zoom = 45.0f;
		}
	}


private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up    = glm::normalize(glm::cross(Right, Front));

		East = glm::normalize(glm::vec3(Right.x, 0, Right.z));
		North = glm::normalize(glm::vec3(Front.x, 0, Front.z));
	}
};
#endif
