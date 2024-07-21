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
        //void begin();
        void StopMainLoop();
        void SetupInputCallbacks();


        //VIEWPORTS, UPDATE & RENDER
        void UpdateBeforeRender();
        void CreateViewport(std::string name, glm::vec3 cameraPosition, CAMERA_CONTROLLERS controller);
        void RenderViewports();


        //GENERACION DE PRIMITIVAS
        void CreatePrefabExternalModel(ImportModelData importModelData);
        void CreatePrefabDot(const glm::vec3& pos, const glm::vec3& polygonColor);
        void CreatePrefabLine(const glm::vec3& point1, const glm::vec3& point2);
        void CreateTriangle(const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3);
        void CreatePrefabCube(glm::vec3 position);
        void CreatePrefabSphere(float radius, unsigned int sectorCount, unsigned int stackCount);
        //void CreateRoof(const std::vector<Vector2d>& points, const std::vector<Vector2d>& holes);

        //ImGUI
        void DrawImGUI();



        // Función estática para obtener la ventana GLFW
        static GLFWwindow* GetWindow()
        {
            return GetInstance().window;
        }


    public:

        //MOUSE PICKING & ImGizmo
        //bool isSelectingObject = false;
        //bool showModelSelectionCombo = false;
        bool usingGizmo = false;

        int currentViewport = 0; //Editor Camera by default
    private:

        

        Ref<Scene> currentScene = nullptr;

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
