#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace libCore
{
    struct Transform 
    {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 eulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

        Transform() {}

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

    struct TransformComponent
    {
        Ref<Transform> transform;

        TransformComponent(Ref<Transform> trans = nullptr)
            : transform(trans) {}
    };

    struct MaterialComponent 
    {
        Ref<Material> material;

        MaterialComponent(Ref<Material> mat = nullptr)
            : material(mat) {}
    };

}