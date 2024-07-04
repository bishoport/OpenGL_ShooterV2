#pragma once
#include "../LibCoreHeaders.h"
#include "../input/InputManager.h"
#include "EngineOpenGL.h"
#include "Viewport.hpp"
#include "../tools/LightsManager.hpp"
#include "../tools/SkyBox.hpp"
#include "IBL.h"
#include <random>
#include <vector>
#include <glm/glm.hpp>
//#include "Itc_matrix.hpp"

namespace libCore {

    class Renderer {

    public:
        //SKYBOX
        Scope<DynamicSkybox> dynamicSkybox = nullptr;
        
        //MISC
        bool m_wireframe = false;
        bool enableMultisample = true;
        
        //IBL
        Scope<IBL> ibl = nullptr;
        bool iblEnabled = true; // Variable para activar/desactivar IBL
        bool dynamicIBL = false;  // Cambia esto a `false` para IBL estático
        float iblIntensity = 0.0f;
        
        //SSAO
        bool ssaoEnabled = true; // Variable para activar/desactivar SSAO
        float ssaoRadius =2.0f;
        float ssaoBias = 0.5f;
        float ssaoIntensity = 1.8f;
        float ssaoPower = 1.8f;
        float F0Factor = 0.04f;
        
        //GLOBAL LIGHT
        float ambientLight = 1.0f;
        
        //HDR
        bool hdrEnabled = false;
        float hdrExposure = 1.0f;



        static Renderer& getInstance() {
            static Renderer instance;
            return instance;
        }



        void initialize() {
            //--SKYBOX
            std::vector<const char*> faces {
                "assets/Skybox/right.jpg",
                    "assets/Skybox/left.jpg",
                    "assets/Skybox/top.jpg",
                    "assets/Skybox/bottom.jpg",
                    "assets/Skybox/front.jpg",
                    "assets/Skybox/back.jpg"
            };
            dynamicSkybox = CreateScope<DynamicSkybox>(faces);
            //-------------------------------------------------------


            //--IBL
            ibl = CreateScope<IBL>();
            ibl->prepareIBL(1080, 720,dynamicIBL);
            
            //-------------------------------------------------------


            //--SSAO
            setupQuad();// Configuración del quad para SSAO
            ssaoKernel = generateSSAOKernel();         // Genera el kernel
            noiseTexture = generateSSAONoiseTexture(); // Genera textura de ruido SSAO
            //-------------------------------------------------------


            //--AREA LIGHT
            //mLTC.mat1 = loadMTexture();
            //mLTC.mat2 = loadLUTTexture();
            //-------------------------------------------------------
        }
        


        void PushDebugGroup(const std::string& name) {
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name.c_str());
        }

        void PopDebugGroup() {
            glPopDebugGroup();
        }




        void RenderViewport(const Ref<Viewport>& viewport, const Timestep& m_deltaTime, const std::vector<Ref<libCore::ModelContainer>>& modelsInScene) {

            if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_P)) 
            {
                m_wireframe = !m_wireframe;
            }

            if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_M))
            {
                enableMultisample = !enableMultisample;
            }
            
            if (enableMultisample)
            {
                glEnable(GL_MULTISAMPLE);
            }
            else 
            {
                glDisable(GL_MULTISAMPLE);
            }

            //// Limpiar el buffer de color y profundidad del framebuffer por defecto
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //// Actualizar la cámara
            viewport->camera->Inputs(m_deltaTime);
            viewport->camera->updateMatrix(45.0f, 0.1f, 1000.0f);

            glEnable(GL_DEPTH_TEST); // Habilitar el test de profundidad





            //--------------------------------------------------------------------------------
    //-----------------------DIRECTIONAL LIGHT SHADOW PASS----------------------------
    //--------------------------------------------------------------------------------
            auto& directionalLight = LightsManager::GetInstance().GetDirectionalLight();

            if (directionalLight != nullptr && directionalLight->drawShadows) {
                PushDebugGroup("Directional Light Shadow Pass");

                // Configura la vista y proyección desde la perspectiva de la luz
                glm::mat4 shadowProjMat = glm::ortho(directionalLight->orthoLeft,
                    directionalLight->orthoRight,
                    directionalLight->orthoBottom,
                    directionalLight->orthoTop,
                    directionalLight->orthoNear,
                    directionalLight->orthoFar);

                glm::mat4 shadowViewMat = glm::lookAt(directionalLight->transform.position,
                    directionalLight->transform.position + directionalLight->direction,
                    glm::vec3(0, 1, 0));

                directionalLight->shadowMVP = shadowProjMat * shadowViewMat;

                // Bind the shadow framebuffer
                viewport->framebuffer_shadowmap->bindFBO();
                glViewport(0, 0, viewport->viewportSize.x, viewport->viewportSize.y);
                glClear(GL_DEPTH_BUFFER_BIT);

                libCore::ShaderManager::Get("direct_light_depth_shadows")->use();
                libCore::ShaderManager::Get("direct_light_depth_shadows")->setMat4("shadowMVP", directionalLight->shadowMVP);

                // Draw the models in the scene
                for (auto& modelContainer : modelsInScene) {
                    modelContainer->Draw("direct_light_depth_shadows");
                }

                viewport->framebuffer_shadowmap->unbindFBO();
                PopDebugGroup();
            }

            glViewport(0, 0, viewport->viewportSize.x, viewport->viewportSize.y);
            //--------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------





            //------------------------------------------------------------------------------------------------------
            //---------------------------------------------DEFERRED-------------------------------------------------
            //------------------------------------------------------------------------------------------------------

            // 1.1 Renderizado de geometría en el GBuffer
            PushDebugGroup("Deferred Geometry Pass");
            viewport->gBuffer->bindGBuffer();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Usar el shader de geometría
            libCore::ShaderManager::Get("geometryPass")->use();
            libCore::ShaderManager::Get("geometryPass")->setMat4("projection", viewport->camera->projection);
            libCore::ShaderManager::Get("geometryPass")->setMat4("view", viewport->camera->view);

            // Configurar el modo de polígono para wireframe o sólido
            if (m_wireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            //// Dibujar los modelos en la escena
            for (auto& modelContainer : modelsInScene) 
            {
                modelContainer->Draw("geometryPass");
            }
            // Desvinculamos el GBuffer
            viewport->gBuffer->unbindGBuffer();
            PopDebugGroup();
            //--------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------





            ////--------------------------------------------------------------------------------
            ////-----------------------SSAO-----------------------------------------------------
            ////--------------------------------------------------------------------------------
            if (ssaoEnabled) 
            {

                // Renderizado SSAO
                PushDebugGroup("SSAO Pass");

                //Copiar el buffer de profundidad del GBuffer al FBO SSAO
                viewport->blitGBO2FBO(viewport->gBuffer, viewport->framebuffer_SSAO, GL_DEPTH_BUFFER_BIT, GL_NEAREST);


                viewport->framebuffer_SSAO->bindFBO();
                glClear(GL_COLOR_BUFFER_BIT); // No se limpia aquí el GL_DEPTH_BUFFER_BIT!!!!
                libCore::ShaderManager::Get("ssao")->use();

                for (unsigned int i = 0; i < 64; ++i)
                    libCore::ShaderManager::Get("ssao")->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);

                libCore::ShaderManager::Get("ssao")->setMat4("projection", viewport->camera->projection);

                libCore::ShaderManager::Get("ssao")->setInt("gPosition", 0);
                libCore::ShaderManager::Get("ssao")->setInt("gPositionView", 1);
                libCore::ShaderManager::Get("ssao")->setInt("gNormal", 2);
                libCore::ShaderManager::Get("ssao")->setInt("texNoise", 3);

                // Establecer valores de radio, sesgo (bias), intensidad y power para SSAO
                libCore::ShaderManager::Get("ssao")->setFloat("radius", ssaoRadius);
                libCore::ShaderManager::Get("ssao")->setFloat("bias", ssaoBias);
                libCore::ShaderManager::Get("ssao")->setFloat("intensity", ssaoIntensity);
                libCore::ShaderManager::Get("ssao")->setFloat("power", ssaoPower);

                libCore::ShaderManager::Get("ssao")->setFloat("screenWidth", viewport->viewportSize.x);
                libCore::ShaderManager::Get("ssao")->setFloat("screenHeight", viewport->viewportSize.y);

                viewport->gBuffer->bindTexture("position", 0);
                viewport->gBuffer->bindTexture("positionView", 1);
                viewport->gBuffer->bindTexture("normal", 2);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, noiseTexture);

                renderQuad();
                viewport->framebuffer_SSAO->unbindFBO();
                PopDebugGroup();
                //--------------------------------------------------------------------------------------------------------------

                //---Renderizado SSAO_BLUR
                PushDebugGroup("SSAO Blur Pass");
                
                //Copiar el buffer de profundidad del GBuffer al FBO SSAOBlur
                viewport->blitGBO2FBO(viewport->gBuffer, viewport->framebuffer_SSAOBlur, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

                viewport->framebuffer_SSAOBlur->bindFBO();
                glClear(GL_COLOR_BUFFER_BIT);
                libCore::ShaderManager::Get("ssaoBlur")->use();
                libCore::ShaderManager::Get("ssaoBlur")->setInt("ssaoInput", 0);
                viewport->framebuffer_SSAO->bindTexture("color", 0);
                renderQuad();
                viewport->framebuffer_SSAOBlur->unbindFBO();
                PopDebugGroup();
                //--------------------------------------------------------------------------------
            }
            //--------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------



            //--------------------------------------------------------------------------------
            //-----------------------LIGHTING PASS--------------------------------------------
            //--------------------------------------------------------------------------------
            PushDebugGroup("Lighting Pass");
            viewport->framebuffer_deferred->bindFBO();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // forzamos el quitar el wireframe para mostrar el quad

            // Pasamos las texturas generadas por el GBuffer-> Shader de iluminación para usar esta información y combinar las luces
            libCore::ShaderManager::Get("lightingPass")->use();
            libCore::ShaderManager::Get("lightingPass")->setInt("gPosition", 0);
            libCore::ShaderManager::Get("lightingPass")->setInt("gNormal", 1);
            libCore::ShaderManager::Get("lightingPass")->setInt("gAlbedo", 2);
            libCore::ShaderManager::Get("lightingPass")->setInt("gMetallicRoughness", 3);
            libCore::ShaderManager::Get("lightingPass")->setInt("gSpecular", 4);
            libCore::ShaderManager::Get("lightingPass")->setInt("ssaoTexture", 5);
            viewport->gBuffer->bindTexture("position", 0);
            viewport->gBuffer->bindTexture("normal", 1);
            viewport->gBuffer->bindTexture("albedo", 2);
            viewport->gBuffer->bindTexture("metallicRoughness", 3);
            viewport->gBuffer->bindTexture("specular", 4);
            viewport->framebuffer_SSAO->bindTexture("color", 5);

            //ALL Lights
            LightsManager::GetInstance().SetLightDataInShader("lightingPass");

            //DIRECTIONAL Light
            if (LightsManager::GetInstance().GetDirectionalLight() != nullptr)
            {
                libCore::ShaderManager::Get("lightingPass")->setMat4("lightSpaceMatrix", directionalLight->shadowMVP);
            }

            libCore::ShaderManager::Get("lightingPass")->setBool("useSSAO", ssaoEnabled);
            libCore::ShaderManager::Get("lightingPass")->setFloat("exposure", hdrExposure);
            libCore::ShaderManager::Get("lightingPass")->setFloat("ambientLight", ambientLight);
            libCore::ShaderManager::Get("lightingPass")->setVec3("viewPos", viewport->camera->Position);
            libCore::ShaderManager::Get("lightingPass")->setFloat("F0Factor", F0Factor);

            //AREA LIGHT
            libCore::ShaderManager::Get("lightingPass")->setInt("LTC1", 6);
            libCore::ShaderManager::Get("lightingPass")->setInt("LTC2", 7);

            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, mLTC.mat1);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, mLTC.mat2);
            //----------------------------------------------------------------------------------------------



            libCore::ShaderManager::Get("lightingPass")->setBool("useIBL", iblEnabled);
            libCore::ShaderManager::Get("lightingPass")->setFloat("iblIntensity", iblIntensity);
            libCore::ShaderManager::Get("lightingPass")->setInt("irradianceMap", 8);
            libCore::ShaderManager::Get("lightingPass")->setInt("prefilterMap", 9);
            libCore::ShaderManager::Get("lightingPass")->setInt("brdfLUT", 10);

            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->irradianceMap);
            glActiveTexture(GL_TEXTURE9);
            glBindTexture(GL_TEXTURE_CUBE_MAP, ibl->prefilterMap);
            glActiveTexture(GL_TEXTURE10);
            glBindTexture(GL_TEXTURE_2D, ibl->brdfLUTTexture);
            //----------------------------------------------------------------------------------------------

            // SHADOW MAP
            if (LightsManager::GetInstance().GetDirectionalLight() != nullptr)
            {
                if (directionalLight->drawShadows)
                {
                    libCore::ShaderManager::Get("lightingPass")->setBool("useShadows", directionalLight->drawShadows); // Activa las sombras
                    libCore::ShaderManager::Get("lightingPass")->setInt("shadowMap", 11);

                    // Utiliza el método bindTexture del FBO
                    viewport->framebuffer_shadowmap->bindTexture("depth", 11);
                }
            }
            renderQuad();

            // Desvincula el FBO deferred
            viewport->framebuffer_deferred->unbindFBO();
            PopDebugGroup();
            //--------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------




            //------------------------------------------------------------------------------------------------------
            //---------------------------------------------FORWARD--------------------------------------------------
            //------------------------------------------------------------------------------------------------------
            PushDebugGroup("Forward Pass");

            viewport->blitGBO2FBO(viewport->gBuffer, viewport->framebuffer_forward, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

            viewport->framebuffer_forward->bindFBO();
            glClear(GL_COLOR_BUFFER_BIT); // No se limpia aquí el GL_DEPTH_BUFFER_BIT!!!!
            glDepthFunc(GL_LESS);
            //------------------------------------------------------------------------------------------


            // PASADA SKYBOX
            dynamicSkybox->Render(viewport->camera->view, viewport->camera->projection);
            //------------------------------------------------------------------------------------------
            
            // PASADA DE DEBUG
            libCore::ShaderManager::Get("debug")->use();
            libCore::ShaderManager::Get("debug")->setMat4("view", viewport->camera->view);
            libCore::ShaderManager::Get("debug")->setMat4("projection", viewport->camera->projection);
            //------------------------------------------------------------------------------------------
             
            //DEBUG de Luces
            LightsManager::GetInstance().DrawDebugLights("debug");
            //------------------------------------------------------------------------------------------

            //DEBUG AABB
            for (auto& modelContainer : modelsInScene)
            {
               // modelContainer->DrawAABB("debug");
            }
            //------------------------------------------------------------------------------------------
            
            // PASADA DE TEXTOS
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glm::mat4 model = glm::mat4(1.0f);
            libCore::ShaderManager::Get("text")->use();
            libCore::ShaderManager::Get("text")->setMat4("projection", viewport->camera->projection);
            libCore::ShaderManager::Get("text")->setMat4("model", model);
            libCore::ShaderManager::Get("text")->setMat4("view", viewport->camera->view);
            //--> Render Textos AQUI!!!
            glDisable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ZERO);
            //------------------------------------------------------------------------------------------

            // Desligar el FBO forward
            viewport->framebuffer_forward->unbindFBO();
            PopDebugGroup();
            //------------------------------------------------------------------------------------------
            //------------------------------------------------------------------------------------------



            //------------------------------------------------------------------------------------------------------
            //------------COMBINE DEFERRED+FORWARD+SSAO+SHADOWS-----------------------------------------------------
            //------------------------------------------------------------------------------------------------------
            // 5. Combinar los resultados en el FBO final
            PushDebugGroup("Combine Pass");
            viewport->framebuffer_final->bindFBO();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            libCore::ShaderManager::Get("combinePass")->use();
            viewport->framebuffer_deferred->bindTexture("color", 0);
            viewport->framebuffer_forward->bindTexture("color", 1);
            viewport->framebuffer_SSAOBlur->bindTexture("color", 2);

            viewport->framebuffer_shadowmap->bindTexture("depth", 3);

            if (LightsManager::GetInstance().GetDirectionalLight() != nullptr)
            {
                if (directionalLight->drawShadows) {
                    viewport->framebuffer_shadowmap->bindTexture("depth", 3);
                    //glActiveTexture(GL_TEXTURE3);
                    //glBindTexture(GL_TEXTURE_2D, directionalLight->shadowTex);
                }
            }
            libCore::ShaderManager::Get("combinePass")->setInt("deferredTexture", 0);
            libCore::ShaderManager::Get("combinePass")->setInt("forwardTexture", 1);
            libCore::ShaderManager::Get("combinePass")->setInt("ssaoTexture", 2);
            libCore::ShaderManager::Get("combinePass")->setInt("shadowTexture", 3);

            libCore::ShaderManager::Get("combinePass")->setBool("useSSAO", ssaoEnabled);
            if (LightsManager::GetInstance().GetDirectionalLight() != nullptr)
            {
                libCore::ShaderManager::Get("combinePass")->setBool("useShadow", directionalLight->drawShadows);
            }
            renderQuad();
            viewport->framebuffer_final->unbindFBO();
            PopDebugGroup();
            //------------------------------------------------------------------------------------------
            //------------------------------------------------------------------------------------------
        }

        void ShowViewportInQuad(Ref<Viewport> viewport) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            libCore::ShaderManager::Get("colorQuadFBO")->use();
            viewport->framebuffer_final->bindTexture("color", 0);
            libCore::ShaderManager::Get("colorQuadFBO")->setInt("screenTexture", 0);
            renderQuad();
        }

        void ShowControlsGUI() {
            // Global Ilumination Controls
            ImGui::Begin("Ilumination Controls");
            
            ImGui::Text("Global Light");
            ImGui::SliderFloat("Ambient Light", &ambientLight, 0.0f, 10.0f, "%.2f");
            ImGui::SliderFloat("HDR Exposure", &hdrExposure, 0.1f, 10.0f, "%.2f");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            
            ImGui::Text("IBL");
            ImGui::Checkbox("Enable IBL", &iblEnabled); // Checkbox para activar/desactivar IBL
            ImGui::SliderFloat("Intensity", &iblIntensity, 0.0f, 10.0f, "%.2f");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            
            ImGui::Text("SSAO");
            ImGui::Checkbox("Enable SSAO", &ssaoEnabled); // Checkbox para activar/desactivar SSAO
            ImGui::SliderFloat("SSAO Radius", &ssaoRadius, 0.1f, 10.0f, "%.2f");
            ImGui::SliderFloat("SSAO Bias", &ssaoBias, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("SSAO Intensity", &ssaoIntensity, 0.0f, 5.0f, "%.2f");
            ImGui::SliderFloat("SSAO Power", &ssaoPower, 0.1f, 5.0f, "%.2f");
            ImGui::SliderFloat("Base Reflectivity", &F0Factor, 0.1f, 5.0f, "%.2f");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            
            ImGui::Text("Dynamic Skybox");
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Checkbox("Use Skybox Texture", &dynamicSkybox->useTexture);
            ImGui::ColorEdit3("dayLightColor", (float*)&dynamicSkybox->dayLightColor);
            ImGui::ColorEdit3("sunsetColor", (float*)&dynamicSkybox->sunsetColor);
            ImGui::ColorEdit3("dayNightColor", (float*)&dynamicSkybox->dayNightColor);
            ImGui::ColorEdit3("groundColor", (float*)&dynamicSkybox->groundColor);
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            


            ImGui::Text("Sun");
            ImGui::DragFloat3("sunPosition", glm::value_ptr(dynamicSkybox->sunPosition), 0.001f);
            float sunDiskSizeValue = dynamicSkybox->m_sunDiskSize.x;  // Asumimos que todos los valores son iguales
            if (ImGui::SliderFloat("Sun disk size", &sunDiskSizeValue, 0.0f, 1.0f, "%.4f")) {
                dynamicSkybox->m_sunDiskSize = glm::vec3(sunDiskSizeValue, sunDiskSizeValue, sunDiskSizeValue);
            }
            ImGui::SliderFloat("Sun disk m_gradientIntensity", &dynamicSkybox->m_gradientIntensity, 0.0f, 10.0f, "%.4f");
            ImGui::SliderFloat("Sun disk auraIntensity",       &dynamicSkybox->auraIntensity, 0.0f, 1.0f, "%.4f");
            ImGui::SliderFloat("Sun disk auraSize",            &dynamicSkybox->auraSize, 0.0f, 1.0f, "%.4f");
            ImGui::SliderFloat("Sun disk edgeSoftness",        &dynamicSkybox->edgeSoftness, 0.0001f, 0.1f, "%.4f");
            ImGui::Dummy(ImVec2(0.0f, 3.0f));

            ImGui::Text("Stars Settings");
            ImGui::SliderFloat("Star Density", &dynamicSkybox->starDensity, 0.0f, 0.01f, "%.5f");
            ImGui::SliderFloat("Star Size Min", &dynamicSkybox->starSizeMin, 0.0f, 1.0f, "%.4f");
            ImGui::SliderFloat("Star Size Max", &dynamicSkybox->starSizeMax, 0.0f, 2.0f, "%.4f");
            ImGui::SliderFloat("Star Brightness Min", &dynamicSkybox->starBrightnessMin, 0.0f, 1.0f, "%.4f");
            ImGui::SliderFloat("Star Brightness Max", &dynamicSkybox->starBrightnessMax, 0.0f, 1.0f, "%.4f");
            ImGui::DragFloat2("Star Coord Scale", glm::value_ptr(dynamicSkybox->starCoordScale), 0.1f, 0.0f, 200.0f, "%.4f");
            
            ImGui::End();
        }

    private:
        Renderer() {} // Constructor privado

        std::vector<glm::vec3> ssaoKernel;
        GLuint noiseTexture;
        const unsigned int NR_LIGHTS = 1;
        GLuint quadVAO, quadVBO;

        void setupQuad() {
            float quadVertices[] = {
                // positions  // texCoords
                -1.0f,  1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                 1.0f, -1.0f, 1.0f, 0.0f,
                 1.0f, -1.0f, 1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f, 1.0f,
                 1.0f,  1.0f, 1.0f, 1.0f
            };

            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glBindVertexArray(0);
        }

        void renderQuad() {
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

        float ourLerp(float a, float b, float f) {
            return a + f * (b - a);
        }

        std::uniform_real_distribution<GLfloat> randomFloats{0.0f, 1.0f}; // Genera floats aleatorios entre 0.0 y 1.0
        std::default_random_engine generator;

        std::vector<glm::vec3> generateSSAOKernel() {
            std::vector<glm::vec3> ssaoKernel;
            for (unsigned int i = 0; i < 64; ++i) {
                glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
                sample = glm::normalize(sample);
                sample *= randomFloats(generator);
                float scale = float(i) / 64.0f;

                // Escalar muestras para que estén más alineadas al centro del kernel
                scale = ourLerp(0.1f, 1.0f, scale * scale);
                sample *= scale;
                ssaoKernel.push_back(sample);
            }
            return ssaoKernel;
        }

        // Generar la textura de ruido SSAO
        GLuint generateSSAONoiseTexture() {
            std::vector<glm::vec3> ssaoNoise;
            for (unsigned int i = 0; i < 16; i++) {
                glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // Rotar alrededor del eje z (en espacio tangente)
                ssaoNoise.push_back(noise);
            }
            unsigned int noiseTexture;
            glGenTextures(1, &noiseTexture);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            return noiseTexture;
        }


        //LTC´s
        struct LTC_matrices {
            GLuint mat1;
            GLuint mat2;
        };

        LTC_matrices mLTC;

        GLuint loadMTexture()
        {
            GLuint texture = 0;
            //glGenTextures(1, &texture);
            //glBindTexture(GL_TEXTURE_2D, texture);

            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, LTC1);

            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //glBindTexture(GL_TEXTURE_2D, 0);
            return texture;
        }

        GLuint loadLUTTexture()
        {
            GLuint texture = 0;
            /*glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, LTC2);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);*/
            return texture;
        }
    };
}