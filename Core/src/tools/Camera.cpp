#include "Camera.h"

namespace libCore
{
    Camera::Camera(int width, int height, glm::vec3 position)
    {
        Camera::width = width;
        Camera::height = height;
        Position = position;
    }

    void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
    {
        // Makes camera look in the right direction from the right position
        view = glm::lookAt(Position, Position + Orientation, Up);
        // Adds perspective to the scene
        projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
        // Sets new camera matrix
        cameraMatrix = projection * view;
    }

    void Camera::Inputs(libCore::Timestep deltaTime)
    {
        // Handles key inputs
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_W))
            Position += speed * Orientation * deltaTime.GetMilliseconds();
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_S))
            Position += speed * -Orientation * deltaTime.GetMilliseconds();
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_A))
            Position += speed * -glm::normalize(glm::cross(Orientation, Up)) * deltaTime.GetMilliseconds();
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_D))
            Position += speed * glm::normalize(glm::cross(Orientation, Up)) * deltaTime.GetMilliseconds();
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_R))
            Position += speed * Up * deltaTime.GetMilliseconds();
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_F))
            Position += speed * -Up * deltaTime.GetMilliseconds();

        // Handles mouse inputs
        if (InputManager::Instance().IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
        {
            // Hides mouse cursor
            glfwSetInputMode(libCore::EngineOpenGL::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            // Prevents camera from jumping on the first click
            if (firstClick)
            {
                glfwSetCursorPos(libCore::EngineOpenGL::GetWindow(), (width / 2), (height / 2));
                firstClick = false;
            }

            // Stores the coordinates of the cursor
            double mouseX;
            double mouseY;
            // Fetches the coordinates of the cursor
            glfwGetCursorPos(libCore::EngineOpenGL::GetWindow(), &mouseX, &mouseY);

            // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
            // and then "transforms" them into degrees 
            float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
            float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

            // Calculates upcoming vertical change in the Orientation
            glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

            // Decides whether or not the next vertical Orientation is legal or not
            if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
            {
                Orientation = newOrientation;
            }

            // Rotates the Orientation left and right
            Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

            // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
            glfwSetCursorPos(libCore::EngineOpenGL::GetWindow(), (width / 2), (height / 2));
        }
        else
        {
            // Unhides cursor since camera is not looking around anymore
            glfwSetInputMode(libCore::EngineOpenGL::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            // Makes sure the next time the camera looks around it doesn't jump
            firstClick = true;
        }
    }
}
