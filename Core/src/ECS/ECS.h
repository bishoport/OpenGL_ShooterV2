#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace libCore
{
    struct Transform {
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




//#pragma once
//
//namespace libCore
//{
//    struct Transform {
//        glm::vec3 position;
//        glm::vec3 rotation;
//        glm::vec3 scale;
//
//        Transform()
//            : position(0.0f, 0.0f, 0.0f),
//            rotation(0.0f, 0.0f, 0.0f),
//            scale(1.0f, 1.0f, 1.0f) {}
//
//        glm::mat4 getMatrix() const {
//            glm::mat4 mat = glm::mat4(1.0f);
//            mat = glm::translate(mat, position);
//            mat = glm::rotate(mat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
//            mat = glm::rotate(mat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
//            mat = glm::rotate(mat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
//            mat = glm::scale(mat, scale);
//            return mat;
//        }
//
//        glm::mat4 getRotationMatrix() const {
//            glm::mat4 mat = glm::mat4(1.0f);
//            mat = glm::rotate(mat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
//            mat = glm::rotate(mat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
//            mat = glm::rotate(mat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
//            return mat;
//        }
//    };
//}


//namespace libCore
//{
//    struct Transform {
//        glm::vec3 position;
//        glm::vec3 rotation; // En radianes
//        glm::vec3 scale;
//
//        Transform()
//            : position(0.0f, 0.0f, 0.0f),
//              rotation(0.0f, 0.0f, 0.0f),
//              scale(1.0f, 1.0f, 1.0f) {}
//
//        glm::mat4 getMatrix() const {
//            glm::mat4 mat = glm::mat4(1.0f);
//            mat = glm::translate(mat, position);
//            mat = glm::rotate(mat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
//            mat = glm::rotate(mat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
//            mat = glm::rotate(mat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
//            mat = glm::scale(mat, scale);
//            return mat;
//        }
//    };
//}