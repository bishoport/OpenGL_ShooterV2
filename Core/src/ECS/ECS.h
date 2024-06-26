#pragma once

namespace libCore
{
    struct Transform {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        Transform()
            : position(0.0f, 0.0f, 0.0f),
            rotation(0.0f, 0.0f, 0.0f),
            scale(1.0f, 1.0f, 1.0f) {}

        glm::mat4 getMatrix() const {
            glm::mat4 mat = glm::mat4(1.0f);
            mat = glm::translate(mat, position);
            mat = glm::rotate(mat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            mat = glm::rotate(mat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            mat = glm::rotate(mat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            mat = glm::scale(mat, scale);
            return mat;
        }

        glm::mat4 getRotationMatrix() const {
            glm::mat4 mat = glm::mat4(1.0f);
            mat = glm::rotate(mat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            mat = glm::rotate(mat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            mat = glm::rotate(mat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            return mat;
        }
    };
}


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