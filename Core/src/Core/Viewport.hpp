#pragma once
#include "../LibCoreHeaders.h"
#include "GBO.hpp"

namespace libCore
{
    class Viewport
    {
    public:
        std::string viewportName = "no_name_viewport";

        Scope<libCore::Camera> camera;
        //Scope<libCore::FBO> framebuffer_shadowmap = nullptr;
        Scope<libCore::FBO> framebuffer_deferred = nullptr;
        Scope<libCore::FBO> framebuffer_forward = nullptr;
        Scope<libCore::FBO> framebuffer_final = nullptr;
        Scope<libCore::FBO> framebuffer_HDR = nullptr;
        Scope<libCore::FBO> framebuffer_SSAO = nullptr;
        Scope<libCore::FBO> framebuffer_SSAOBlur = nullptr;

        Scope<libCore::GBO> gBuffer = nullptr;

        ImVec2 viewportSize = ImVec2(800.0f, 600.0f);
        ImVec2 viewportPos = ImVec2(0.0f, 0.0f);
        ImVec2 previousViewportSize = ImVec2(800.0f, 600.0f);
        ImVec2 previousViewportPos = ImVec2(0.0f, 0.0f);

        // Banderas de estado
        bool isResizing = false;
        bool isMoving = false;
        bool mouseInviewport = false;

        // Constructor predeterminado
        Viewport()
        {
            //--RESIZE SCENE PANEL EVENT
            EventManager::OnPanelResizedEvent().subscribe([this](const std::string name, const glm::vec2& size, const glm::vec2& position)
                {
                    //std::cout << "RESIZE SCENE PANEL EVENT" << std::endl;
                    viewportSize.x = size.x;
                    viewportSize.y = size.y;

                    viewportPos.x = position.x;
                    viewportPos.y = viewportSize.y - position.y;

                    gBuffer->resize(size.x, size.y);

                    framebuffer_deferred->resize(size.x, size.y);
                    framebuffer_forward->resize(size.x, size.y);
                    framebuffer_final->resize(size.x, size.y);
                    framebuffer_HDR->resize(size.x, size.y);
                    framebuffer_SSAO->resize(size.x, size.y);
                    framebuffer_SSAOBlur->resize(size.x, size.y);  // Añadido para SSAO Blur

                    camera->width = static_cast<int>(size.x);
                    camera->height = static_cast<int>(size.y);
                });

            EventManager::OnWindowResizeEvent().subscribe([this](const int width, const int height)
                {
                    //std::cout << "RESIZE WINDOW EVENT" << std::endl;
                    viewportSize.x = width;
                    viewportSize.y = height;

                    gBuffer->resize(width, height);

                    framebuffer_deferred->resize(width, height);
                    framebuffer_forward->resize(width, height);
                    framebuffer_final->resize(width, height);
                    framebuffer_HDR->resize(width, height);
                    framebuffer_SSAO->resize(width, height);
                    framebuffer_SSAOBlur->resize(width, height);  // Añadido para SSAO Blur

                    camera->width = static_cast<int>(width);
                    camera->height = static_cast<int>(height);
                });
            //---------------------------------------------------
        }

        Viewport(const std::string& name) :viewportName(name), viewportSize(0, 0), viewportPos(0, 0), previousViewportSize(0, 0), previousViewportPos(0, 0), mouseInviewport(false) {}
    };
}
