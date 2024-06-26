#pragma once

#include "Sprite3D.h"

#include <memory>
#include "../Common_Structs.h"

namespace Game
{
    struct TileUVData
    {
        float u1 = 0.0f;
        float v1 = 0.0f;
        float u2 = 0.0f;
        float v2 = 0.0f;

        float W = 0.0f;
        float w = 0.0f;

        float H = 0.0f;
        float h = 0.0f;
    };

    class Tile{

    public:
        Tile();
        void SetupTile();
        void SetPosition(float x, float y); // Define la posición del tile
        void SetTexture(std::shared_ptr<libCore::Texture> texture); // Asocia una textura con el tile

        void Draw(); // Sobreescribe el método Draw para usar las coordenadas UV y la textura

        glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f); // Almacenar la posición del tile

        TileUVData tileUVData;

    private:
        float uvCoords[8]; // Almacenar las coordenadas UV para este tile
        std::shared_ptr<libCore::Texture> texture; // Textura asociada al tile
        GLuint VAO, VBO;
    };
}
