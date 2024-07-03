#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <glad/glad.h>
#include <limits>
#include <ApproxMVBB/ComputeApproxMVBB.hpp>

namespace libCore {
    class OBB {
    private:
        glm::mat3 m_rot;   // Rotation matrix for the OBB
        glm::vec3 m_pos;   // Center of the OBB
        glm::vec3 m_ext;   // Extents of the OBB
        mutable GLuint VBO = 0, VAO = 0, EBO = 0;  // OpenGL handles

        void setupBuffers() const {
            std::vector<glm::vec3> vertices(8);
            glm::vec3 axes[3] = {
                m_ext.x * m_rot[0],
                m_ext.y * m_rot[1],
                m_ext.z * m_rot[2]
            };

            // Definir los vértices de la OBB
            vertices[0] = m_pos - axes[0] - axes[1] - axes[2];
            vertices[1] = m_pos + axes[0] - axes[1] - axes[2];
            vertices[2] = m_pos + axes[0] + axes[1] - axes[2];
            vertices[3] = m_pos - axes[0] + axes[1] - axes[2];
            vertices[4] = m_pos - axes[0] - axes[1] + axes[2];
            vertices[5] = m_pos + axes[0] - axes[1] + axes[2];
            vertices[6] = m_pos + axes[0] + axes[1] + axes[2];
            vertices[7] = m_pos - axes[0] + axes[1] + axes[2];

            GLuint indices[] = {
                0, 1, 1, 2, 2, 3, 3, 0,   // Cara inferior
                4, 5, 5, 6, 6, 7, 7, 4,   // Cara superior
                0, 4, 1, 5, 2, 6, 3, 7    // Aristas verticales
            };

            if (VAO == 0) {
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glGenBuffers(1, &EBO);
            }

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

    public:
        ~OBB() {
            if (VAO) {
                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);
                glDeleteBuffers(1, &EBO);
            }
        }

        void build_from_points(const std::vector<glm::vec3>& points) {
            if (points.empty()) return;

            ApproxMVBB::Matrix3Dyn pointsMatrix(3, points.size());
            for (size_t i = 0; i < points.size(); ++i) {
                pointsMatrix(0, i) = points[i].x;
                pointsMatrix(1, i) = points[i].y;
                pointsMatrix(2, i) = points[i].z;
            }

            // Calcular el OBB utilizando ApproxMVBB
            auto oobb = ApproxMVBB::approximateMVBB(pointsMatrix, 0.001, 500, 5, 0, 5);
            oobb.expandToMinExtentRelative(0.1);

            // Extraer datos del OBB calculado
            m_pos = glm::vec3(oobb.center()(0), oobb.center()(1), oobb.center()(2));
            m_ext = glm::vec3(oobb.extent()(0), oobb.extent()(1), oobb.extent()(2));

            m_rot = glm::mat3(
                glm::vec3(oobb.m_q_KI.matrix()(0, 0), oobb.m_q_KI.matrix()(1, 0), oobb.m_q_KI.matrix()(2, 0)),
                glm::vec3(oobb.m_q_KI.matrix()(0, 1), oobb.m_q_KI.matrix()(1, 1), oobb.m_q_KI.matrix()(2, 1)),
                glm::vec3(oobb.m_q_KI.matrix()(0, 2), oobb.m_q_KI.matrix()(1, 2), oobb.m_q_KI.matrix()(2, 2))
            );
        }

        void draw() const {
            if (VAO == 0) {
                setupBuffers();
            }

            glBindVertexArray(VAO);
            glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    };
}
