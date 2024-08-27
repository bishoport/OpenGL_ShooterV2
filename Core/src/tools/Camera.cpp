#include "Camera.h"

namespace libCore
{
    Camera::Camera(int width, int height, glm::vec3 position)
    {
        this->width = width;
        this->height = height;
        this->Position = position;
        this->OrientationQuat = glm::quat(Orientation); // Inicializar cuaternión desde los ángulos de Euler
    }

    void Camera::updateMatrix()
    {
        // Limitar el pitch para evitar el Gimbal Lock
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        // Actualizar la orientación usando los ángulos de Euler en yaw y pitch
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Orientation = glm::normalize(front);

        // Actualizar el cuaternión
        OrientationQuat = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f));

        this->view = glm::lookAt(Position, Position + Orientation, Up);

        // Seleccionar la matriz de proyección en función del modo
        if (isOrthographic)
        {
            float orthoSize = 10.0f; // Tamaño de la cámara ortográfica (ajustable)
            this->projection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
        }
        else
        {
            this->projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
        }

        this->cameraMatrix = projection * view;
    }

    void Camera::UpdateOrientationFromEuler()
    {
        // Convertir la orientación de Euler a un cuaternión
        this->OrientationQuat = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f));
    }

    void Camera::UpdateOrientationFromQuaternion()
    {
        // Convertir el cuaternión a ángulos de Euler
        glm::vec3 euler = glm::eulerAngles(OrientationQuat);
        this->pitch = glm::degrees(euler.x);
        this->yaw = glm::degrees(euler.y);
    }

    void libCore::Camera::LookAt(const glm::vec3& targetPosition)
    {
        // Calcula la dirección hacia el objetivo
        glm::vec3 direction = glm::normalize(targetPosition - Position);

        // Calcula el nuevo pitch y yaw
        pitch = glm::degrees(asin(direction.y)); // Pitch: inclinación hacia arriba o abajo
        yaw = glm::degrees(atan2(direction.z, direction.x)); // Yaw: rotación alrededor del eje Y

        // Limitar el pitch para evitar el Gimbal Lock
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        // Ajustar la orientación usando los nuevos valores de pitch y yaw
        updateMatrix();
    }
}
