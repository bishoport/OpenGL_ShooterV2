// EditorCamera.cpp
#include "EditorCamera.h"

namespace libCore
{
    EditorCamera::EditorCamera(int width, int height, glm::vec3 position)
        : Camera(width, height, position)
    {
    }

    void EditorCamera::Inputs(libCore::Timestep deltaTime)
    {
        // Lógica de entradas para la cámara de edición (idéntica a tu clase original)
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

        if (InputManager::Instance().IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
        {
            glfwSetInputMode(libCore::EngineOpenGL::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            if (firstClick)
            {
                glfwSetCursorPos(libCore::EngineOpenGL::GetWindow(), (width / 2), (height / 2));
                firstClick = false;
            }

            double mouseX;
            double mouseY;
            glfwGetCursorPos(libCore::EngineOpenGL::GetWindow(), &mouseX, &mouseY);

            float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
            float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

            glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

            if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
            {
                Orientation = newOrientation;
            }

            Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

            glfwSetCursorPos(libCore::EngineOpenGL::GetWindow(), (width / 2), (height / 2));
        }
        else
        {
            glfwSetInputMode(libCore::EngineOpenGL::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstClick = true;
        }
    }
}

