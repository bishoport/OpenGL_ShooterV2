#pragma once

#include "../LibCoreHeaders.h"
#include "../src/Core/EngineOpenGL.h"
#include "../Timestep.h"
#include "../input/InputManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace libCore
{
    class Camera
    {
    public:
        glm::vec3 Position;
        glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 cameraMatrix = glm::mat4(1.0f);

        int width;
        int height;

        float speed = 0.06f;
        float sensitivity = 100.0f;

        float FOVdeg = 45.0f;
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;

        Camera(int width, int height, glm::vec3 position);

        void updateMatrix();
        virtual void Inputs(libCore::Timestep deltaTime) = 0; // Método virtual puro para entradas específicas
    };
}