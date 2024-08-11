#include "Camera.h"
namespace libCore
{
    Camera::Camera(int width, int height, glm::vec3 position)
    {
        this->width = width;
        this->height = height;
        this->Position = position;
    }

    void Camera::updateMatrix()
    {
        this->view = glm::lookAt(Position, Position + Orientation, Up);
        this->projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
        this->cameraMatrix = projection * view;
    }
}