#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace libCore {
    class Transform {
    public:
        // Inicializa la Transformaci�n
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

        glm::mat4 getMatrix() const {
            return getLocalModelMatrix();
        }

        glm::mat4 getRotationMatrix() const {
            return glm::toMat4(rotation);
        }

        void setMatrix(const glm::mat4& matrix) {
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(matrix, scale, rotation, position, skew, perspective);
            // Normalizar la rotaci�n
            rotation = glm::normalize(rotation);
            // Actualizar �ngulos de Euler
            eulerAngles = glm::eulerAngles(rotation);
        }

        void updateRotationFromEulerAngles() {
            rotation = glm::quat(eulerAngles);
            rotation = glm::normalize(rotation);
        }

        // Funciones para obtener y establecer la posici�n
        glm::vec3 GetPosition() const { return position; }
        void SetPosition(const glm::vec3& pos) { position = pos; }

        // Funciones para obtener y establecer la rotaci�n
        glm::quat GetRotation() const { return rotation; }
        void SetRotation(const glm::quat& rot) {
            rotation = rot;
            rotation = glm::normalize(rotation);
            eulerAngles = glm::eulerAngles(rotation);
        }
        void SetRotation(const glm::vec3& euler) {
            eulerAngles = euler;
            updateRotationFromEulerAngles();
        }

        // Funciones para obtener y establecer la escala
        glm::vec3 GetScale() const { return scale; }
        void SetScale(const glm::vec3& scl) { scale = scl; }

        // Funci�n para obtener y establecer los �ngulos de Euler
        glm::vec3 GetEulerAngles() const { return eulerAngles; }
        void SetEulerAngles(const glm::vec3& euler) {
            eulerAngles = euler;
            updateRotationFromEulerAngles();
        }
    };
}
