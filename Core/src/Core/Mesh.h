#pragma once
#include "../LibCoreHeaders.h"
#include "VAO.h"
#include "EBO.h"
#include "Transform.h"

namespace libCore
{
    class Mesh
    {
    public:
        Mesh() = default;

        //--MAIN MESH DATA
        std::string meshName = "unknown_mesh_name";
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        DRAW_GEOM_LIKE drawLike = DRAW_GEOM_LIKE::TRIANGLE;

        //--DRAW MESH
        void SetupMesh();
        void Draw();
        void DrawInstanced(GLsizei instanceCount, const std::vector<glm::mat4>& instanceMatrices);
        void SetupInstanceVBO(const std::vector<glm::mat4>& instanceMatrices);

    private:
        //--MAIN MESH
        VAO VAO;

        // Instancing data
        GLuint instanceVBO = 0;
    };
}
