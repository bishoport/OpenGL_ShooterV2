#pragma once

#include "../LibCoreHeaders.h"
#include "../Timestep.h"
#include "../tools/AssetsManager.h"
#include "../tools/ModelLoader.h"
#include "../tools/PrimitivesHelper.h"
#include "Light.hpp"
#include "../StraightSkeleton/Vector2d.h"
#include "../tools/FreeTypeManager.h"
#include "../ECS/Scene.h"


namespace libCore
{
    enum EngineStates
    {
        EDITOR,
        EDITOR_PLAY,
        PLAY
    };

    class EngineOpenGL
    {
    public:
        static EngineOpenGL& GetInstance()
        {
            static EngineOpenGL instance;
            return instance;
        }


        //ENGINE LIFE--CYCLE
        EngineStates engineState = EngineStates::EDITOR;
        bool InitializeEngine(const std::string& windowTitle, int initialWindowWidth, int initialWindowHeight);
        void InitializeMainLoop();
        void StopMainLoop();
        void SetupInputCallbacks();


        //VIEWPORTS, UPDATE & RENDER
        void UpdateBeforeRender();
        void CreateViewport(std::string name, glm::vec3 cameraPosition, CAMERA_CONTROLLERS controller);
        void RenderViewports();


        // Función estática para obtener la ventana GLFW
        static GLFWwindow* GetWindow()
        {
            return GetInstance().window;
        }


    public:
        bool usingGizmo = false;
        int currentViewport = 0; //Editor Camera by default
        Scope<Scene> currentScene = nullptr;

    private:
        GLFWwindow* window = nullptr;
        Timestep m_deltaTime = 0.0f;

        bool running = false;

        bool mouseInImGUI = false;
        int windowWidth = 0;
        int windowHeight = 0;



        // Constructor privado para el patrón Singleton
        EngineOpenGL(){};

        // Deshabilitar copia y asignación
        EngineOpenGL(const EngineOpenGL&) = delete;
        EngineOpenGL& operator=(const EngineOpenGL&) = delete;
    };
}
