#pragma once

#include "../LibCoreHeaders.h"

#include "../Core/FBO.hpp"
#include "../tools/EventManager.h"
#include "../input/InputManager.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../Timestep.h"
#include "../tools/Camera.h"
#include "../Core/Viewport.hpp"
#include "../StraightSkeleton/Vector2d.h"

#include "AssetsPanel.h"

namespace libCore
{
    class GuiLayer {

    public:

        bool drawImGUI = true;
        bool isSelectingObject = false;
        bool showModelSelectionCombo = false;

        using CallbackFromGuiLayer = std::function<void(const std::vector<Vector2d>&, const std::vector<Vector2d>&)>;

        GuiLayer(GLFWwindow* window, float window_W, float window_H);
        void SetCallBackFunc(CallbackFromGuiLayer callbackFromGuiLayerFunc);
        ~GuiLayer();

        void begin();
        void Draw();
        void end();
        void renderDockers();
        void renderMainMenuBar();


		//Panels
        void DrawSelectedEntityComponentsPanel();
        void DrawEntityNode(entt::entity entity);
		void DrawHierarchyPanel();
        void DrawLightsPanel(const std::vector<Ref<libCore::Light>>& lightsInScene);
        void DrawMaterialsPanel();
        void ShowTexturesPanel();

        void ShowModelInfo(const Ref<libCore::Model>& model, int depth = 0);
        void ShowModelsPanel();

        void ShowTexture(const char* label, Ref<Texture> texture);
        void RenderCheckerMatrix();
        void DrawComponentEditor(entt::entity entity);
        void DrawToolBarEditor();

        void ShowLogPanel();

        void checkGizmo(const Ref<Viewport>& viewport);

        //void DrawMaterial(const Ref<Material> materialData);

        enum class GizmoOperation
        {
            Translate,
            Rotate2D, Rotate3D,
            Scale
        };
        GizmoOperation m_GizmoOperation;

    private:
        // Variables globales para controlar el estado del popup
        bool allowPopupClose = false;
        bool popupJustClosed = false;
        std::chrono::steady_clock::time_point popupOpenTime;
        std::chrono::steady_clock::time_point popupCloseTime;

        CallbackFromGuiLayer m_callbackFromGuiLayerFunc;
        std::vector<std::pair<int, int>> selectedOrder;
        bool ini_file_exists;
        float m_window_W = 0.0f;
        float m_window_H = 0.0f;

        struct Point {
            float x, y;
        };


        //PANELS
        Scope<AssetsPanel> assetsPanel = nullptr;
    };	
}