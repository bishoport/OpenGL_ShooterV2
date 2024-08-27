#pragma once

#include "../LibCoreHeaders.h"
#include "../src/Core/EngineOpenGL.h"
#include "../Timestep.h"
#include "../input/InputManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace libCore
{
    class Camera
    {
    public:
        glm::vec3 Position;
        glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f); // Ángulos de Euler
        glm::quat OrientationQuat = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)); // Cuaternión
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 cameraMatrix = glm::mat4(1.0f);

        int width;
        int height;

        float speed = 0.06f;
        float sensitivity = 100.0f;

        float yaw = -90.0f;   // Inicialmente apuntando hacia el frente
        float pitch = 0.0f;   // Inicialmente sin inclinación
        float FOVdeg = 45.0f;
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;

        bool isOrthographic = false; // Nuevo: Controla si la cámara está en modo ortográfico

        Camera(int width, int height, glm::vec3 position);

        void updateMatrix();
        virtual void Inputs(libCore::Timestep deltaTime) = 0; // Método virtual puro para entradas específicas

        void UpdateOrientationFromEuler();
        void UpdateOrientationFromQuaternion();

        // Nueva función para que la cámara mire a un punto específico
        void LookAt(const glm::vec3& targetPosition);
    };
}
