#pragma once

#include "../LibCoreHeaders.h"
#include "../Timestep.h"
#include "../tools/AssetsManager.h"
#include "../tools/ModelLoader.h"
#include "../tools/PrimitivesHelper.h"
#include "Light.hpp"
#include "../StraightSkeleton/Vector2d.h"
#include "../tools/FreeTypeManager.h"


namespace libCore
{
    class EngineOpenGL
    {
        using MainLoopFnc = std::function<void(Timestep)>;
        using CloseGlFnc = std::function<void()>;
        using KeyPressedFnc = std::function<void(int)>;
        using ImGUILoopFnc = std::function<void()>;

    public:
        //LIFE CYCLE
        bool InitializeEngine(const std::string& windowTitle, int initialWindowWidth, int initialWindowHeight, MainLoopFnc mainLoopFnc, CloseGlFnc closeGlFnc, ImGUILoopFnc imGUILoopFnc = [] {});
        void InitializeMainLoop();
        void begin();
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
        void CreateRoof(const std::vector<Vector2d>& points, const std::vector<Vector2d>& holes);

        //PANELS
        void DrawHierarchyPanel();

        // Función estática para obtener la instancia de EngineOpenGL
        static EngineOpenGL& GetInstance()
        {
            static EngineOpenGL instance;
            return instance;
        }

        // Función estática para obtener la ventana GLFW
        static GLFWwindow* GetWindow()
        {
            return GetInstance().window;
        }


    public:
        FreeTypeManager* freeTypeManager = nullptr;

        std::vector<Ref<Model>> modelsInRay;
        Ref<Model> currentSelectedModelInScene = nullptr;
        bool isSelectingObject = false;
        bool showModelSelectionCombo = false;

    private:

        int currentViewport = 0; //Editor Camera by default

        std::vector<Ref<libCore::Model>> modelsInScene;

        GLFWwindow* window;
        Timestep m_deltaTime;

        MainLoopFnc g_mainLoodFnc;
        CloseGlFnc g_closeGlFnc;
        KeyPressedFnc g_keyPressedFnc;
        ImGUILoopFnc g_imGUILoopFnc;

        libCore::ShaderManager shaderManager;

        bool running = false;
        bool useImGUI = true;
        bool renderInImGUI = false;

        bool mouseInImGUI = false;

        int windowWidth = 0;
        int windowHeight = 0;

        
        void checkRayModelIntersection(const Ref<libCore::Model>& model, const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::mat4& accumulatedTransform);


        // Constructor privado para el patrón Singleton
        EngineOpenGL() = default;

        // Deshabilitar copia y asignación
        EngineOpenGL(const EngineOpenGL&) = delete;
        EngineOpenGL& operator=(const EngineOpenGL&) = delete;
    };
}
