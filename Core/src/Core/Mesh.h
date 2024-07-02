#pragma once
#include "../LibCoreHeaders.h"
#include<string>
#include<vector>

#include "VAO.h"
#include "EBO.h"
#include "Texture.h"
#include <Eigen/Dense>

namespace libCore
{
    class Mesh
    {
    public:
        // Initializes the mesh
        Mesh() = default;

        std::string meshName = "unknow_mesh_name";
        glm::vec3 meshLocalPosition = glm::vec3(0.0f);

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        DRAW_GEOM_LIKE drawLike = DRAW_GEOM_LIKE::TRIANGLE;

        glm::vec3 minBounds, maxBounds; // Límites en espacio local
        glm::vec3 worldMinBounds, worldMaxBounds; // Límites en espacio mundial

        void SetupMesh();

        void Draw();

        
        void CalculateAABB();
        void PrepareAABB();
        
        void UpdateAABB(const glm::mat4& modelMatrix);
        
        void DrawAABB();

        bool IsPointInsideAABB(const glm::vec3& point);


    private:
        VAO VAO;

        GLuint VBO_BB, VAO_BB, EBO_BB;
    };
}
