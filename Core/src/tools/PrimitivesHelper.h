#pragma once

#include "../LibCoreHeaders.h"
#include "../Core/Mesh.h"

namespace libCore 
{
    class PrimitivesHelper 
    {

    public:
        static Ref<Mesh> CreateQuad();
        static Ref<Mesh> CreatePlane();
        static Ref<Mesh> CreateCube();
        static Ref<Mesh> CreateSphere(float radius, unsigned int sectorCount, unsigned int stackCount);
        static Ref<Mesh> CreateDot();
        static Ref<Mesh> CreateLine(const glm::vec3& point1, const glm::vec3& point2);
        static Ref<Mesh> CreateTriangle(const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3);
        static Ref<Mesh> CreatePolygon(const std::vector<glm::vec3>& positions);
    };
}