#pragma once
#include <LibCore.h>
#include <map>

namespace Game 
{
    class Sprite3D
    {
    public:
        Sprite3D() = default;
        virtual ~Sprite3D() = default;

        virtual void Update(libCore::Timestep deltaTime) = 0;
        virtual void Draw() = 0;

        glm::vec3 m_position{ -10.0f, 5.0f, 0.0f };

    protected:
        unsigned int quadVAO = 0;
        unsigned int quadVBO = 0;
    };
}
