// FPSCamera.h
#pragma once
#include "Camera.h"

namespace libCore
{
    class FPSCamera : public Camera
    {
    public:
        bool isJumping = false;
        float jumpSpeed = 10.0f;
        float gravity = -19.81f;
        float verticalVelocity = 0.0f;

        FPSCamera(int width, int height, glm::vec3 position);

        void Inputs(libCore::Timestep deltaTime) override;

    private:
        float pitch = 0.0f;
        float yaw = -90.0f; // Inicializar mirando hacia adelante en el eje Z negativo

        float floorLimit = 0.0f;
    };
}

