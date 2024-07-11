#include "EditorCamera.h"

namespace libCore
{
    EditorCamera::EditorCamera(int width, int height, glm::vec3 position)
        : Camera(width, height, position)
    {
    }

    void EditorCamera::Inputs(libCore::Timestep deltaTime)
    {
        // Movimiento de la c�mara
        float velocity = speed * deltaTime.GetMilliseconds();
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_W))
            Position += velocity * Orientation;
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_S))
            Position -= velocity * Orientation;
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_A))
            Position -= glm::normalize(glm::cross(Orientation, Up)) * velocity;
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_D))
            Position += glm::normalize(glm::cross(Orientation, Up)) * velocity;
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_R))
            Position += Up * velocity;
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_F))
            Position -= Up * velocity;

        // Rotaci�n de la c�mara
        if (InputManager::Instance().IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
        {
            glfwSetInputMode(libCore::EngineOpenGL::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            if (firstClick)
            {
                glfwSetCursorPos(libCore::EngineOpenGL::GetWindow(), (width / 2), (height / 2));
                firstClick = false;
            }

            double mouseX, mouseY;
            glfwGetCursorPos(libCore::EngineOpenGL::GetWindow(), &mouseX, &mouseY);

            float rotX = sensitivity * static_cast<float>(mouseY - (height / 2)) / height;
            float rotY = sensitivity * static_cast<float>(mouseX - (width / 2)) / width;

            // Calcular la nueva orientaci�n
            glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
            Orientation = glm::rotate(Orientation, glm::radians(-rotX), right);
            Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

            // Normalizar la orientaci�n
            Orientation = glm::normalize(Orientation);

            // Resetear la posici�n del cursor
            glfwSetCursorPos(libCore::EngineOpenGL::GetWindow(), (width / 2), (height / 2));
        }
        else
        {
            glfwSetInputMode(libCore::EngineOpenGL::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstClick = true;
        }

        // Actualizar la matriz de c�mara
        updateMatrix(45.0f, 0.1f, 100.0f); // Puedes ajustar los par�metros de FOV, nearPlane y farPlane seg�n sea necesario
    }
}


