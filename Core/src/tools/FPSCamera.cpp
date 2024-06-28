// FPSCamera.cpp
#include "FPSCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>

namespace libCore
{
    FPSCamera::FPSCamera(int width, int height, glm::vec3 position) : Camera(width, height, position)
    {
        floorLimit = position.y;

        // Inicializaci�n inicial de la orientaci�n de la c�mara
        yaw = -90.0f; // Inicialmente mirando hacia adelante en el eje Z negativo
        pitch = 0.0f;
        Orientation = glm::vec3(
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        );
    }

    void FPSCamera::Inputs(libCore::Timestep deltaTime)
    {
        // L�gica de entradas para la c�mara FPS
        float moveSpeed = speed * deltaTime.GetMilliseconds();

        // Movimiento adelante y atr�s
        glm::vec3 forward = glm::normalize(glm::vec3(Orientation.x, 0.0f, Orientation.z));
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_W))
            Position += moveSpeed * forward;
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_S))
            Position -= moveSpeed * forward;

        // Movimiento izquierda y derecha
        glm::vec3 right = glm::normalize(glm::cross(forward, Up));
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_A))
            Position -= moveSpeed * right;
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_D))
            Position += moveSpeed * right;

        // Manejo de saltos
        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_SPACE) && !isJumping)
        {
            isJumping = true;
            verticalVelocity = jumpSpeed;
        }

        // Aplicar gravedad
        if (isJumping)
        {
            verticalVelocity += gravity * deltaTime.GetSeconds();
            Position.y += verticalVelocity * deltaTime.GetSeconds();

            // Si toca el suelo
            if (Position.y <= floorLimit)
            {
                Position.y = floorLimit;
                isJumping = false;
                verticalVelocity = 0.0f;
            }
        }

        // Manejo del rat�n para rotar la c�mara
        glfwSetInputMode(libCore::EngineOpenGL::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        double mouseX, mouseY;
        glfwGetCursorPos(libCore::EngineOpenGL::GetWindow(), &mouseX, &mouseY);

        float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
        float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

        // Actualizar yaw y pitch
        yaw += rotY;
        pitch -= rotX;

        // Limitar pitch para evitar que la c�mara se voltee
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // Calcular la nueva orientaci�n
        Orientation = glm::vec3(
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        );

        // Volver a centrar el cursor
        glfwSetCursorPos(libCore::EngineOpenGL::GetWindow(), (width / 2), (height / 2));
    }
}
