#pragma once

#include "../LibCoreHeaders.h"
#include "../src/Core/EngineOpenGL.h"
#include "../Timestep.h"
#include "../input/InputManager.h"

namespace libCore
{
    class Camera
    {
	public:
		// Stores the main vectors of the camera
		glm::vec3 Position;
		glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 cameraMatrix = glm::mat4(1.0f);

		// Prevents the camera from jumping around when first clicking left click
		bool firstClick = true;

		// Stores the width and height of the window
		int width;
		int height;

		// Adjust the speed of the camera and it's sensitivity when looking around
		float speed = 0.06f;
		float sensitivity = 100.0f;

		// Camera constructor to set up initial values
		Camera(int width, int height, glm::vec3 position);

		// Updates the camera matrix to the Vertex Shader
		void updateMatrix(float FOVdeg, float nearPlane, float farPlane);

		// Handles camera inputs
		void Inputs(libCore::Timestep deltaTime);
	};
}
