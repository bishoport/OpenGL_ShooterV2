#pragma once

#include "../LibCoreHeaders.h"
#include "Viewport.hpp"
#include "FBO.hpp"
//#include "../tools/Camera.h"
#include "../tools/EditorCamera.h"
#include "../tools/FPSCamera.h"

namespace libCore
{
    class ViewportManager {

    public:

        std::vector<Ref<Viewport>> viewports;

        void CreateViewport(std::string name, glm::vec3 cameraPosition, int viewportWidth, int viewportHeight, CAMERA_CONTROLLERS controller)
        {
            auto viewport = CreateRef<Viewport>();
            viewport->viewportName = name;
            viewport->viewportSize.x = viewportWidth;
            viewport->viewportSize.y = viewportHeight;

            // Camera
            if (controller == CAMERA_CONTROLLERS::FPS)
            {
                viewport->camera = CreateScope<libCore::FPSCamera>(viewport->viewportSize.x, viewport->viewportSize.y, cameraPosition);
            }
            else if (controller == CAMERA_CONTROLLERS::EDITOR)
            {
                viewport->camera = CreateScope<libCore::EditorCamera>(viewport->viewportSize.x, viewport->viewportSize.y, cameraPosition);
            }
            
            //----------------------------------------------------------


            // G-Buffer
            auto gbo = CreateScope<GBO>();
            gbo->init(viewport->viewportSize.x, viewport->viewportSize.y);
            viewport->gBuffer = std::move(gbo);
            //----------------------------------------------------------


            // F-Buffer Deferred + lighting
            auto fbo0 = CreateScope<FBO>();
            fbo0->init(viewport->viewportSize.x, viewport->viewportSize.y, GL_RGB16, "F-Buffer Deferred-lighting");
            fbo0->addAttachment("color", GL_RGB16F, GL_RGB, GL_FLOAT);
            fbo0->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
            fbo0->closeSetup();
            viewport->framebuffer_deferred = std::move(fbo0);
            //----------------------------------------------------------

            // F-Buffer Deferred + Forward 
            auto fbo1 = CreateScope<FBO>();
            fbo1->init(viewport->viewportSize.x, viewport->viewportSize.y, GL_RGB8, "F-Buffer Deferred-Forward");
            fbo1->addAttachment("color", GL_RGB8, GL_RGB, GL_FLOAT);
            fbo1->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
            fbo1->closeSetup();
            viewport->framebuffer_forward = std::move(fbo1);
            //----------------------------------------------------------

            // F-Buffer Final
            auto fbo2 = CreateScope<FBO>();
            fbo2->init(viewport->viewportSize.x, viewport->viewportSize.y, GL_RGB8, "F-Buffer Final");
            fbo2->addAttachment("color", GL_RGB8, GL_RGB, GL_FLOAT);
            fbo2->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
            fbo2->closeSetup();
            viewport->framebuffer_final = std::move(fbo2);
            //----------------------------------------------------------

            // F-Buffer HDR
            auto fbo3 = CreateScope<FBO>();
            fbo3->init(viewport->viewportSize.x, viewport->viewportSize.y, GL_RGB16F, "F-Buffer HDR"); // Este buffer tiene una coma flotante más grande porque es para HDR
            fbo3->addAttachment("color", GL_RGB16F, GL_RGB, GL_FLOAT);
            fbo3->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
            fbo3->closeSetup();
            viewport->framebuffer_HDR = std::move(fbo3);
            //----------------------------------------------------------

            // F-Buffer SSAO
            auto fbo4 = CreateScope<FBO>();
            fbo4->init(viewport->viewportSize.x, viewport->viewportSize.y, GL_RED, "F-Buffer SSAO");
            fbo4->addAttachment("color", GL_RED, GL_RED, GL_FLOAT);
            fbo4->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
            fbo4->closeSetup();
            viewport->framebuffer_SSAO = std::move(fbo4);
            //----------------------------------------------------------

            // F-Buffer SSAO-BLUR
            auto fbo5 = CreateScope<FBO>();
            fbo5->init(viewport->viewportSize.x, viewport->viewportSize.y, GL_RED, "F-Buffer SSAO-BLUR");
            fbo5->addAttachment("color", GL_RED, GL_RED, GL_FLOAT);
            fbo5->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
            fbo5->closeSetup();
            viewport->framebuffer_SSAOBlur = std::move(fbo5);
            //----------------------------------------------------------


            //// Directional Light ShadowMap
            //auto fbo6 = CreateScope<FBO>();
            //fbo6->init(viewport->viewportSize.x, viewport->viewportSize.y, GL_RGB16F, "Directional Light ShadowMap");
            //fbo6->addAttachment("color", GL_RGB16F, GL_RGB, GL_FLOAT);
            //fbo6->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
            //fbo6->closeSetup();
            //viewport->framebuffer_shadowmap = std::move(fbo6);
            ////----------------------------------------------------------

            // Add Viewport to collection
            viewports.push_back(std::move(viewport));
   
        }

        void DrawPanelGUI()
        {
            ImGui::Begin("Framebuffers");

            if (ImGui::CollapsingHeader("GBuffer Textures")) {
                GLuint gBufferPositionTexture = viewports[0]->gBuffer->getTexture("position");
                ImGui::Text("Position");
                ImGui::Image((void*)(intptr_t)gBufferPositionTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

                GLuint gBufferDepthTexture = viewports[0]->gBuffer->getTexture("depth");
                ImGui::Text("depth");
                ImGui::Image((void*)(intptr_t)gBufferDepthTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

                GLuint gBufferPositionViewTexture = viewports[0]->gBuffer->getTexture("positionView");
                ImGui::Text("positionView");
                ImGui::Image((void*)(intptr_t)gBufferPositionViewTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

                GLuint gBufferNormalTexture = viewports[0]->gBuffer->getTexture("normal");
                ImGui::Text("Normal");
                ImGui::Image((void*)(intptr_t)gBufferNormalTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

                GLuint gBufferAlbedoTexture = viewports[0]->gBuffer->getTexture("albedo");
                ImGui::Text("Albedo");
                ImGui::Image((void*)(intptr_t)gBufferAlbedoTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

                GLuint gMetallicRoughnessTexture = viewports[0]->gBuffer->getTexture("metallicRoughness");
                ImGui::Text("Metallic_Roughness");
                ImGui::Image((void*)(intptr_t)gMetallicRoughnessTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

                GLuint gSpecularTexture = viewports[0]->gBuffer->getTexture("specular");
                ImGui::Text("Specular");
                ImGui::Image((void*)(intptr_t)gSpecularTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
            }

            if (ImGui::CollapsingHeader("Framebuffers")) 
            {
                //GLuint shadowMapTexture = viewports[0]->framebuffer_shadowmap->getTexture("color");
                //ImGui::Text("ShadowMap Texture");
                //ImGui::Image((void*)(intptr_t)shadowMapTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

                GLuint ssaoBlurTexture = viewports[0]->framebuffer_SSAOBlur->getTexture("color");
                ImGui::Text("SSAO Blur Texture");
                ImGui::Image((void*)(intptr_t)ssaoBlurTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

                GLuint deferredTexture = viewports[0]->framebuffer_deferred->getTexture("color");
                ImGui::Text("Deferred FBO");
                ImGui::Image((void*)(intptr_t)deferredTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

                GLuint forwardTexture = viewports[0]->framebuffer_forward->getTexture("color");
                ImGui::Text("Forward FBO");
                ImGui::Image((void*)(intptr_t)forwardTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));


            }
            ImGui::End();
        }
    };
}


