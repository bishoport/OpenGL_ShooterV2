#include "GameApp.h"

namespace Game
{

    void GameApp::Init()
    {
        //--SETUP OPENGL & CALLBACKS
        bool ok = libCore::InitializeEngine("2D Platform Game", 1080, 720,
            std::bind(&GameApp::LoopOpenGL, this, std::placeholders::_1),
            std::bind(&GameApp::OnCloseOpenGL, this),
            std::bind(&GameApp::LoopImGUI, this));
        if (!ok) return;
        //-----------------------------------------------------------------

        //--OpenGL FLAGS
            //glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //---------------------------------------------------------------------------

        //-- SHADERS
        std::string shadersDirectory = "Assets/shaders/";
        shaderManager.setShaderDataLoad("base", shadersDirectory + "Base.vs", shadersDirectory + "Base.fs");
        shaderManager.setShaderDataLoad("debug", shadersDirectory + "Base.vs", shadersDirectory + "Debug.fs");
        shaderManager.LoadAllShaders();
        //-----------------------------------------------------------------

        //-- CAMERA
        m_camera = new libCore::Camera(800, 600, glm::vec3(0.0f, 0.0f, 5.0f));
        //-----------------------------------------------------------------


        ////-- PHYSICS
        b2Vec2 gravity(0.0f, -10.0f); // Gravedad hacia abajo
        world = new b2World(gravity);


        ////-- PLAYER
        CharacterData data{ "Nombre del Personaje" };
        player = new Character(data, world);




        //GenerateMap();
        //GenerateLevel(Game::ImageLoader::LoadTexture("Assets/maps/TestMap2/static_layer.png"), false);
        //GenerateLevel(Game::ImageLoader::LoadTexture("Assets/maps/TestMap2/dinamic_layer.png"), true);

        //--START LOOP OpenGL
        libCore::InitializeMainLoop();
        //----------------------------------------------------------------------------
    }

    void GameApp::LoopOpenGL(libCore::Timestep deltaTime)
    {

        // Update CAMERA
        m_camera->Inputs(deltaTime);
        m_camera->updateMatrix(45.0f, 0.1f, 100.0f);


        // Clear
        glClearColor(0.3, 0.1, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        ////UPDATE
        player->Update(deltaTime);

        libCore::ShaderManager::Get("debug")->use();
        libCore::ShaderManager::Get("debug")->setMat4("view", m_camera->view);
        libCore::ShaderManager::Get("debug")->setMat4("projection", m_camera->projection);

        libCore::ShaderManager::Get("base")->use();
        libCore::ShaderManager::Get("base")->setMat4("view", m_camera->view);
        libCore::ShaderManager::Get("base")->setMat4("projection", m_camera->projection);

        player->Draw();
    }

    void GameApp::OnCloseOpenGL()
    {
    }

    void GameApp::LoopImGUI()
    {
    }

    void GameApp::GenerateLevel(Ref<Texture> atlasTexture, bool pyshics)
    {
        //for (int y = 0; y < atlasTexture->image->height; y += tileHeight) {
        //    for (int x = 0; x < atlasTexture->image->width; x += tileWidth) {
        //        bool hasNonAlpha = false;
        //        for (int dy = 0; dy < tileHeight; ++dy) {
        //            for (int dx = 0; dx < tileWidth; ++dx) {
        //                int pixelIndex = ((y + dy) * atlasTexture->image->width + (x + dx)) * atlasTexture->image->channels;

        //                if (atlasTexture->image->pixels[pixelIndex + 3] != 0) { // Comprobar el canal alfa
        //                    hasNonAlpha = true;
        //                    break;
        //                }
        //            }
        //            if (hasNonAlpha) {
        //                break;
        //            }
        //        }


        //        if (hasNonAlpha) {
        //            TileQuad quad;
        //            quad.m_position = glm::vec3((-x / 5.0f) / tileWidth, (y / 5.0f) / tileHeight, 0); // Centrar el quad en el bloque
        //            // Inicializa la física para este TileQuad
        //            quad.InitializePhysics(world, 0.1f, 0.1f, pyshics);
        //            quads.push_back(quad);
        //        }
        //    }
        //}
    }
}
