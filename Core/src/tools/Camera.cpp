#include "Camera.h"

namespace libCore
{
    Camera::Camera(int width, int height, glm::vec3 position)
    {
        this->width = width;
        this->height = height;
        this->Position = position;
        this->OrientationQuat = glm::quat(Orientation); // Inicializar cuaterni�n desde los �ngulos de Euler
    }

    void Camera::updateMatrix()
    {
        // Limitar el pitch para evitar el Gimbal Lock
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        // Actualizar la orientaci�n usando los �ngulos de Euler en yaw y pitch
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Orientation = glm::normalize(front);

        // Actualizar el cuaterni�n
        OrientationQuat = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f));

        this->view = glm::lookAt(Position, Position + Orientation, Up);

        // Seleccionar la matriz de proyecci�n en funci�n del modo
        if (isOrthographic)
        {
            float orthoSize = 10.0f; // Tama�o de la c�mara ortogr�fica (ajustable)
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
        // Convertir la orientaci�n de Euler a un cuaterni�n
        this->OrientationQuat = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f));
    }

    void Camera::UpdateOrientationFromQuaternion()
    {
        // Convertir el cuaterni�n a �ngulos de Euler
        glm::vec3 euler = glm::eulerAngles(OrientationQuat);
        this->pitch = glm::degrees(euler.x);
        this->yaw = glm::degrees(euler.y);
    }

    void libCore::Camera::LookAt(const glm::vec3& targetPosition)
    {
        // Calcula la direcci�n hacia el objetivo
        glm::vec3 direction = glm::normalize(targetPosition - Position);

        // Calcula el nuevo pitch y yaw
        pitch = glm::degrees(asin(direction.y)); // Pitch: inclinaci�n hacia arriba o abajo
        yaw = glm::degrees(atan2(direction.z, direction.x)); // Yaw: rotaci�n alrededor del eje Y

        // Limitar el pitch para evitar el Gimbal Lock
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        // Ajustar la orientaci�n usando los nuevos valores de pitch y yaw
        updateMatrix();
    }
}
