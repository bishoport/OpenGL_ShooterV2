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
#include "Viewport.hpp"
#include "../StraightSkeleton/Vector2d.h"

namespace libCore
{
    class GuiLayer {

    public:

        using CallbackFromGuiLayer = std::function<void(const std::vector<Vector2d>&, const std::vector<Vector2d>&)>;

        GuiLayer(GLFWwindow* window, float window_W, float window_H);
        void SetCallBackFunc(CallbackFromGuiLayer callbackFromGuiLayerFunc);
        ~GuiLayer();

        void begin();
        void end();
        void renderDockers();
        void renderMainMenuBar();

        //Viewports
        void DrawViewports(std::vector<Ref<Viewport>> viewports);

		//Panels
		void DrawHierarchyPanel(const std::vector<Ref<libCore::ModelContainer>>& modelsInScene);
        void DrawLightsPanel(const std::vector<Ref<libCore::Light>>& lightsInScene);

        void RenderCheckerMatrix();

    private:

        CallbackFromGuiLayer m_callbackFromGuiLayerFunc;
        std::vector<std::pair<int, int>> selectedOrder;
        bool ini_file_exists;
        float m_window_W = 0.0f;
        float m_window_H = 0.0f;

        struct Point {
            float x, y;
        };
    };	
}