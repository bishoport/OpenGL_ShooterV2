#pragma once

#include "../LibCoreHeaders.h"
#include "Viewport.hpp"
#include "FBO.hpp"
#include "../tools/EditorCamera.h"
#include "../tools/GameCamera.h"
#include "Renderer.hpp"

namespace libCore
{
    class ViewportManager {

    public:

        static ViewportManager& GetInstance() {
            static ViewportManager instance;
            return instance;
        }

        std::vector<Ref<Viewport>> viewports;

        void CreateViewport(std::string name, glm::vec3 cameraPosition, int viewportWidth, int viewportHeight, CAMERA_CONTROLLERS controller)
        {
            auto viewport = CreateRef<Viewport>();
            viewport->viewportName = name;
            viewport->viewportSize.x = static_cast<float>(viewportWidth);
            viewport->viewportSize.y = static_cast<float>(viewportHeight);

            // Cameras
            viewport->gameCamera   = CreateRef<libCore::GameCamera>();
            viewport->editorCamera = CreateRef<libCore::EditorCamera>(viewport->viewportSize.x, viewport->viewportSize.y, cameraPosition);

            viewport->camera = viewport->editorCamera;

            viewport->camera->SetPosition(glm::vec3(-20.0f, 8.0f, -20.0f));
            viewport->camera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
            //----------------------------------------------------------
           

            // Camera
            /*if (controller == CAMERA_CONTROLLERS::GAME)
            {
                viewport->camera = CreateRef<libCore::GameCamera>(viewport->viewportSize.x, viewport->viewportSize.y, cameraPosition);
            }
            else if (controller == CAMERA_CONTROLLERS::EDITOR)
            {
                viewport->camera = CreateRef<libCore::EditorCamera>(viewport->viewportSize.x, viewport->viewportSize.y, cameraPosition);
            }*/
            //----------------------------------------------------------


            // G-Buffer
            auto gbo = CreateRef<GBO>();
            gbo->init(static_cast<int>(viewport->viewportSize.x), static_cast<int>(viewport->viewportSize.y));
            viewport->gBuffer = std::move(gbo);
            //----------------------------------------------------------



            // F-Buffer Deferred + lighting
            auto fbo0 = CreateRef<FBO>();
            fbo0->init(static_cast<int>(viewport->viewportSize.x), static_cast<int>(viewport->viewportSize.y), GL_RGB16, "F-Buffer Deferred-lighting", true, false, false);
            fbo0->addAttachment("color", GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
            fbo0->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
            fbo0->closeSetup();
            viewport->framebuffer_deferred = std::move(fbo0);
            //----------------------------------------------------------

            // F-Buffer Deferred + Forward 
            auto fbo1 = CreateRef<FBO>();
            fbo1->init(static_cast<int>(viewport->viewportSize.x), static_cast<int>(viewport->viewportSize.y), GL_RGB8, "F-Buffer Deferred-Forward", true, false, false);
            fbo1->addAttachment("color", GL_RGB8, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
            fbo1->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
            fbo1->closeSetup();
            viewport->framebuffer_forward = std::move(fbo1);
            //----------------------------------------------------------

            // F-Buffer Final
            auto fbo2 = CreateRef<FBO>();
            fbo2->init(static_cast<int>(viewport->viewportSize.x), static_cast<int>(viewport->viewportSize.y), GL_RGB8, "F-Buffer Final", true, false, false);
            fbo2->addAttachment("color", GL_RGB8, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
            fbo2->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
            fbo2->closeSetup();
            viewport->framebuffer_final = std::move(fbo2);
            //----------------------------------------------------------

            // F-Buffer HDR
            auto fbo3 = CreateRef<FBO>();
            fbo3->init(static_cast<int>(viewport->viewportSize.x), static_cast<int>(viewport->viewportSize.y), GL_RGB16F, "F-Buffer HDR", true, false, false); // Este buffer tiene una coma flotante más grande porque es para HDR
            fbo3->addAttachment("color", GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
            fbo3->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
            fbo3->closeSetup();
            viewport->framebuffer_HDR = std::move(fbo3);
            //----------------------------------------------------------

            // F-Buffer SSAO
            auto fbo4 = CreateRef<FBO>();
            fbo4->init(static_cast<int>(viewport->viewportSize.x), static_cast<int>(viewport->viewportSize.y), GL_RED, "F-Buffer SSAO", true, false, false);
            fbo4->addAttachment("color", GL_RED, GL_RED, GL_FLOAT, GL_COLOR_ATTACHMENT0);
            fbo4->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
            fbo4->closeSetup();
            viewport->framebuffer_SSAO = std::move(fbo4);
            //----------------------------------------------------------

            // F-Buffer SSAO-BLUR
            auto fbo5 = CreateRef<FBO>();
            fbo5->init(static_cast<int>(viewport->viewportSize.x), static_cast<int>(viewport->viewportSize.y), GL_RED, "F-Buffer SSAO-BLUR", true, false, false);
            fbo5->addAttachment("color", GL_RED, GL_RED, GL_FLOAT, GL_COLOR_ATTACHMENT0);
            fbo5->addAttachment("depth", GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
            fbo5->closeSetup();
            viewport->framebuffer_SSAOBlur = std::move(fbo5);
            //----------------------------------------------------------

            // F-Buffer ShadowMap
            auto fbo6 = CreateRef<FBO>();
            fbo6->init(static_cast<int>(viewport->viewportSize.x), static_cast<int>(viewport->viewportSize.y), GL_DEPTH_COMPONENT, "F-Buffer ShadowMap", true, false, false);
            fbo6->addAttachment("depth", GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
            fbo6->closeSetup();
            viewport->framebuffer_shadowmap = std::move(fbo6);
            //----------------------------------------------------------

            // Add Viewport to collection
            viewports.push_back(std::move(viewport));
        }

        GLuint CubemapFaceTo2DTexture(GLuint cubemap, GLenum face, int width, int height, GLuint captureFBO)
        {
            GLuint texture2D;
            glGenTextures(1, &texture2D);
            glBindTexture(GL_TEXTURE_2D, texture2D);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face, cubemap, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            return texture2D;
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

            //if (ImGui::CollapsingHeader("IBL"))
            //{
            //    ImGui::Text("envCubemapTexture");
            //    ImGui::Image((void*)(intptr_t)Renderer::getInstance().ibl->envCubemap, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

            //    ImGui::Text("irradianceMap");
            //    ImGui::Image((void*)(intptr_t)Renderer::getInstance().ibl->irradianceMap, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

            //    ImGui::Text("prefilterMap");
            //    ImGui::Image((void*)(intptr_t)Renderer::getInstance().ibl->prefilterMap, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

            //    ImGui::Text("brdfLUTTexture");
            //    ImGui::Image((void*)(intptr_t)Renderer::getInstance().ibl->brdfLUTTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
            //}

            if (ImGui::CollapsingHeader("Framebuffers")) 
            {
                GLuint shadowMapTexture = viewports[0]->framebuffer_shadowmap->getTexture("depth");
                ImGui::Text("Shadow Map Texture");
                ImGui::Image((void*)(intptr_t)shadowMapTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

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


        private:
            ViewportManager() {};
    };
}


