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
        
        Mesh() = default;

        //--DATA
        bool showAABB = false;
        std::string meshName = "unknow_mesh_name";
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        DRAW_GEOM_LIKE drawLike = DRAW_GEOM_LIKE::TRIANGLE;
        glm::vec3 minBounds, maxBounds; // Límites en espacio local
        glm::vec3 worldMinBounds, worldMaxBounds; // Límites en espacio mundial


        //--MAIN MESH
        void SetupMesh();
        void Draw();

        
        //--AABB
        void CalculateAABB();
        void PrepareAABB();
        void UpdateAABB(const glm::mat4& modelMatrix);
        void DrawAABB();
        bool IsPointInsideAABB(const glm::vec3& point);


    private:
        //--MAIN MESH
        VAO VAO;

        //--AABB
        GLuint VBO_BB, VAO_BB, EBO_BB;
    };
}
