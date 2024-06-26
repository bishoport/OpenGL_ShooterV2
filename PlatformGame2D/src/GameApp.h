#pragma once

#include <LibCore.h>
#include <tools/ShaderManager.h>
#include <tools/Camera.h>

#include "GameObjects/Tile.h"
#include "GameObjects/TileQuad.h"
#include "GameObjects/Character.h"
#include <LDtkLoader/Project.hpp>
#include <box2d/include/box2d/box2d.h>

namespace Game
{
    struct Particle {
        glm::vec3 position;
        glm::vec3 velocity;
        float lifetime;
        // Otros atributos como color, tamaño, etc.
    };

    class GameApp
    {
    public:
        GameApp() = default;

        void Init();

    private:


        libCore::ShaderManager shaderManager;

        float lastFrameTime = 0.0f;

        libCore::Camera* m_camera = nullptr;
        void LoopOpenGL(libCore::Timestep deltaTime);
        void OnCloseOpenGL();
        void LoopImGUI();


        Character* player = nullptr;

        //Maps
        void GenerateLevel(Ref<Texture> atlasTexture, bool pyshics);
        std::vector<Tile> tiles; // Colección de tiles

        float tileWidth = 5.0f;
        float tileHeight = 5.0f;
        std::vector<TileQuad> quads;
        std::vector<TileQuad> quadsStatics;

        b2World* world;

    };
}