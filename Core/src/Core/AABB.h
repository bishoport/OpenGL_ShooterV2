#pragma once
#include "../LibCoreHeaders.h"
#include "VAO.h"
#include "EBO.h"


namespace libCore
{
    class AABB
    {
    public:
        AABB() = default;

        bool showAABB = false;
        glm::vec3 minBounds, maxBounds; // L�mites en espacio local
        glm::vec3 worldMinBounds, worldMaxBounds; // L�mites en espacio mundial

        void CalculateAABB(std::vector<Vertex> vertices);
        void PrepareAABB();
        void UpdateAABB(const glm::mat4& modelMatrix);
        void DrawAABB();
        bool IsPointInsideAABB(const glm::vec3& point);

    private:
        GLuint VBO_BB, VAO_BB, EBO_BB;
    };
}

