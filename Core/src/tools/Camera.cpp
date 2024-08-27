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
        // Actualizar la orientación usando los ángulos de Euler en yaw y pitch
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Orientation = glm::normalize(front);

        // Actualizar el cuaternión
        OrientationQuat = glm::quat(glm::vec3(pitch, yaw, 0.0f));

        this->view = glm::lookAt(Position, Position + Orientation, Up);
        this->projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
        this->cameraMatrix = projection * view;
    }

    void Camera::UpdateOrientationFromEuler()
    {
        // Convertir la orientación de Euler a un cuaternión
        this->OrientationQuat = glm::quat(glm::vec3(pitch, yaw, 0.0f));
    }

    void Camera::UpdateOrientationFromQuaternion()
    {
        // Convertir el cuaternión a ángulos de Euler
        glm::vec3 euler = glm::eulerAngles(OrientationQuat);
        this->pitch = glm::degrees(euler.x);
        this->yaw = glm::degrees(euler.y);
    }
}
