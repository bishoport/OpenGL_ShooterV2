#pragma once
#include "../LibCoreHeaders.h"


namespace libCore
{
    class Transform
    {
    public:
        // Initializes the mesh
        Transform() = default;

        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 eulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);


        glm::mat4 getLocalModelMatrix() const {
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
            glm::mat4 rotationMatrix = glm::toMat4(rotation);
            glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
            return translationMatrix * rotationMatrix * scaleMatrix;
        }

        glm::mat4 getMatrix() const
        {
            return getLocalModelMatrix();
        }

        glm::mat4 getRotationMatrix() const
        {
            return glm::toMat4(rotation);
        }

        void setMatrix(const glm::mat4& matrix) {
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(matrix, scale, rotation, position, skew, perspective);
            // Normalizar la rotación
            rotation = glm::normalize(rotation);
            // Actualizar ángulos de Euler
            eulerAngles = glm::eulerAngles(rotation);
        }

        void updateRotationFromEulerAngles() {
            rotation = glm::quat(eulerAngles);
            rotation = glm::normalize(rotation);
        }
    };
}
