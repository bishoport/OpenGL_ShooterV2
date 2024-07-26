#include "Camera.h"
namespace libCore
{
    Camera::Camera(int width, int height, glm::vec3 position)
    {
        this->width = width;
        this->height = height;
        Position = position;
    }

    void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
    {
        view = glm::lookAt(Position, Position + Orientation, Up);
        projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
        cameraMatrix = projection * view;
    }
}