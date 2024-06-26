#pragma once

#include <LibCore.h>
#include <memory>
#include "../Common_Structs.h"
#include "Sprite3D.h"
#include <box2d/include/box2d/box2d.h>



namespace Game {

    class TileQuad : public Sprite3D {
    public:
        TileQuad();
        ~TileQuad();

        void Update(libCore::Timestep deltaTime) override;
        void Draw() override; // Sobreescribe el método Draw para usar las coordenadas UV y la textura

        glm::vec3 centro  = glm::vec3(0.0f, 0.0f, 0.0f);


        //--PYSHICS
        AABB box;
        b2Body* body;
        void InitializePhysics(b2World* world, float tileWidth, float tileHeight, bool pyshics);

    private:
        float quadVertices[12] = {
            -0.1f,  0.1f, 0.0f, // Superior izquierdo
            -0.1f, -0.1f, 0.0f, // Inferior izquierdo
             0.1f,  0.1f, 0.0f, // Superior derecho
             0.1f, -0.1f, 0.0f, // Inferior derecho
        };


        //--DEBUG
        GLuint lineVAO = 0, lineVBO = 0; // Recursos OpenGL para las líneas de la AABB
        void UpdateAABB(); // Actualiza la AABB y los vértices de la línea
        void DrawAABB(); // Dibuja las líneas de la AABB
    };
}

