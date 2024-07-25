#pragma once
#include "../LibCoreHeaders.h"
#include "VAO.h"
#include "EBO.h"


namespace libCore
{
    class Mesh
    {
    public:
        
        Mesh() = default;

        //--MAIN MESH DATA
        std::string meshName = "unknow_mesh_name";
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        DRAW_GEOM_LIKE drawLike = DRAW_GEOM_LIKE::TRIANGLE;

        //--DRAW MESH
        void SetupMesh();
        void Draw();
        void DrawInstanced(GLsizei instanceCount);
    private:
        //--MAIN MESH
        VAO VAO;

        // Instancing data
        GLuint instanceVBO = 0;
        std::vector<glm::mat4> instanceMatrices;
    };
}
