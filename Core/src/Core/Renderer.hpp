#pragma once
#include "../LibCoreHeaders.h"
#include "../input/InputManager.h"
#include "EngineOpenGL.h"
#include "Viewport.hpp"
#include "../tools/LightsManager.hpp"
#include "../tools/SkyBox.hpp"
#include <random>
#include <vector>
#include <glm/glm.hpp>
#include "Itc_matrix.hpp"

namespace libCore {
    class Renderer {
    public:
        Scope<DynamicSkybox> dynamicSkybox = nullptr;

        bool m_wireframe = false;
        bool ssaoEnabled = true; // Variable para activar/desactivar SSAO
        float ssaoRadius = 0.5f;
        float ssaoBias = 0.025f;
        float ssaoIntensity = 1.0f;
        float ssaoPower = 1.0f;
        float F0Factor = 0.04f;
        float ambientLight = 1.0f;
        bool hdrEnabled = false;
        float hdrExposure = 1.0f;

        Renderer() {
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

            // Configuraci�n del quad para SSAO
            setupQuad();

            // Generar el kernel y la textura de ruido SSAO
            ssaoKernel = generateSSAOKernel();
            noiseTexture = generateSSAONoiseTexture();

            //Area Light Config
            mLTC.mat1 = loadMTexture();
            mLTC.mat2 = loadLUTTexture();
        }





        void RenderViewport(const Ref<Viewport>& viewport, const Timestep& m_deltaTime, const std::vector<Ref<libCore::ModelContainer>>& modelsInScene) {
            if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_P)) {
                m_wireframe = !m_wireframe;
            }

            glEnable(GL_DEPTH_TEST); // Habilitar el test de profundidad

            // Limpiar el buffer de color y profundidad del framebuffer por defecto
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //------------------------------------------------------------------------------------------------------
            //---------------------------------------------DEFERRED-------------------------------------------------
            //------------------------------------------------------------------------------------------------------

            // 1.1 Renderizado de geometr�a en el GBuffer
            viewport->gBuffer->bindGBuffer();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Actualizar la c�mara
            viewport->camera->Inputs(m_deltaTime);
            viewport->camera->updateMatrix(45.0f, 0.1f, 1000.0f);

            // Usar el shader de geometr�a
            libCore::ShaderManager::Get("geometryPass")->use();
            libCore::ShaderManager::Get("geometryPass")->setMat4("projection", viewport->camera->projection);
            libCore::ShaderManager::Get("geometryPass")->setMat4("view", viewport->camera->view);

            // Configurar el modo de pol�gono para wireframe o s�lido
            if (m_wireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            // Dibujar los modelos en la escena
            for (auto& modelContainer : modelsInScene) 
            {
                modelContainer->Draw("geometryPass");
            }
            // Desvinculamos el GBuffer
            viewport->gBuffer->unbindGBuffer();
            //--------------------------------------------------------------------------------



            if (ssaoEnabled) {
                // Renderizado SSAO

                // 1.2 Copiar el buffer de profundidad del GBuffer al FBO SSAO
                glBindFramebuffer(GL_READ_FRAMEBUFFER, viewport->gBuffer->gBuffer);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, viewport->framebuffer_SSAO->getFramebuffer());
                glBlitFramebuffer(0, 0, viewport->viewportSize.x, viewport->viewportSize.y, 0, 0, viewport->viewportSize.x, viewport->viewportSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind all

                viewport->framebuffer_SSAO->bindFBO();
                glClear(GL_COLOR_BUFFER_BIT); // No se limpia aqu� el GL_DEPTH_BUFFER_BIT!!!!
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
                //--------------------------------------------------------------------------------------------------------------

                //---Renderizado SSAO_BLUR
                // 1.3 Copiar el buffer de profundidad del GBuffer al FBO SSAOBlur
                glBindFramebuffer(GL_READ_FRAMEBUFFER, viewport->gBuffer->gBuffer);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, viewport->framebuffer_SSAOBlur->getFramebuffer());
                glBlitFramebuffer(0, 0, viewport->viewportSize.x, viewport->viewportSize.y, 0, 0, viewport->viewportSize.x, viewport->viewportSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind all

                viewport->framebuffer_SSAOBlur->bindFBO();
                glClear(GL_COLOR_BUFFER_BIT);
                libCore::ShaderManager::Get("ssaoBlur")->use();
                libCore::ShaderManager::Get("ssaoBlur")->setInt("ssaoInput", 0);
                viewport->framebuffer_SSAO->bindTexture("color", 0);
                renderQuad();
                viewport->framebuffer_SSAOBlur->unbindFBO();
                //--------------------------------------------------------------------------------
            }



            // 2.1 Renderizado de iluminaci�n en el FBO deferred + iluminaci�n
            viewport->framebuffer_deferred->bindFBO();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // forzamos el quitar el wireframe para mostrar el quad

            // Pasamos las texturas generadas por el GBuffer-> Shader de iluminaci�n para usar esta informaci�n y combinar las luces
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

            // Lights
            LightsManager::GetInstance().SetLightDataInShader("lightingPass");

            // Global Scene, CameraView, SSAO
            libCore::ShaderManager::Get("lightingPass")->setBool("useSSAO", ssaoEnabled);
            libCore::ShaderManager::Get("lightingPass")->setFloat("exposure", hdrExposure);
            libCore::ShaderManager::Get("lightingPass")->setFloat("ambientLight", ambientLight);
            libCore::ShaderManager::Get("lightingPass")->setVec3("viewPos", viewport->camera->Position);
            libCore::ShaderManager::Get("lightingPass")->setFloat("F0Factor", F0Factor);

            libCore::ShaderManager::Get("lightingPass")->setInt("LTC1", 6);
            libCore::ShaderManager::Get("lightingPass")->setInt("LTC2", 7);

            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, mLTC.mat1);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, mLTC.mat2);

            renderQuad();

            // Desvincula el FBO deferred
            viewport->framebuffer_deferred->unbindFBO();

            // 3. Copiar el buffer de profundidad del GBuffer al FBO forward
            glBindFramebuffer(GL_READ_FRAMEBUFFER, viewport->gBuffer->gBuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, viewport->framebuffer_forward->getFramebuffer());
            glBlitFramebuffer(0, 0, viewport->viewportSize.x, viewport->viewportSize.y, 0, 0, viewport->viewportSize.x, viewport->viewportSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind all

            // 4. Renderizado de luces de depuraci�n en el FBO forward
            viewport->framebuffer_forward->bindFBO();
            glClear(GL_COLOR_BUFFER_BIT); // No se limpia aqu� el GL_DEPTH_BUFFER_BIT!!!!

            glDepthFunc(GL_LESS);





            //------------------------------------------------------------------------------------------------------
            //---------------------------------------------FORWARD--------------------------------------------------
            //------------------------------------------------------------------------------------------------------

            // PASADA SKYBOX
            //dynamicSkybox->Render(viewport->camera->view, viewport->camera->projection);

            // PASADA DE DEBUG
            libCore::ShaderManager::Get("debug")->use();
            libCore::ShaderManager::Get("debug")->setMat4("view", viewport->camera->view);
            libCore::ShaderManager::Get("debug")->setMat4("projection", viewport->camera->projection);

            //DEBUG de Luces
            LightsManager::GetInstance().DrawDebugLights("debug");
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
            //------------------------------------------------------------------------------------------



            //------------------------------------------------------------------------------------------------------
            //------------COMBINE DEFERRED+FORWARD+SSAO-------------------------------------------------------------
            //------------------------------------------------------------------------------------------------------

            // 5. Combinar los resultados en el FBO final
            viewport->framebuffer_final->bindFBO();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            libCore::ShaderManager::Get("combinePass")->use();
            viewport->framebuffer_deferred->bindTexture("color", 0);
            viewport->framebuffer_forward->bindTexture("color", 1);
            viewport->framebuffer_SSAOBlur->bindTexture("color", 2);
            libCore::ShaderManager::Get("combinePass")->setInt("deferredTexture", 0);
            libCore::ShaderManager::Get("combinePass")->setInt("forwardTexture", 1);
            libCore::ShaderManager::Get("combinePass")->setInt("ssaoTexture", 2);
            libCore::ShaderManager::Get("combinePass")->setBool("useSSAO", ssaoEnabled);
            renderQuad();
            viewport->framebuffer_final->unbindFBO();
            //------------------------------------------------------------------------------------------
        }

        void ShowViewportInQuad(Ref<Viewport> viewport) {
            //-6. Renderizar el resultado final en pantalla
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
            ImGui::SliderFloat("Ambient Light", &ambientLight, 0.0f, 10.0f);
            ImGui::SliderFloat("HDR Exposure", &hdrExposure, 0.1f, 10.0f);
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("SSAO");
            ImGui::Checkbox("Enable SSAO", &ssaoEnabled); // Checkbox para activar/desactivar SSAO
            ImGui::SliderFloat("SSAO Radius", &ssaoRadius, 0.1f, 10.0f);
            ImGui::SliderFloat("SSAO Bias", &ssaoBias, 0.0f, 0.1f);
            ImGui::SliderFloat("SSAO Intensity", &ssaoIntensity, 0.0f, 5.0f);
            ImGui::SliderFloat("SSAO Power", &ssaoPower, 0.1f, 5.0f);
            ImGui::SliderFloat("Base Reflectivity", &F0Factor, 0.1f, 5.0f);
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
            ImGui::SliderFloat("Sun disk auraIntensity", &dynamicSkybox->auraIntensity, 0.0f, 1.0f, "%.4f");
            ImGui::SliderFloat("Sun disk auraSize", &dynamicSkybox->auraSize, 0.0f, 1.0f, "%.4f");
            ImGui::SliderFloat("Sun disk edgeSoftness", &dynamicSkybox->edgeSoftness, 0.0001f, 0.1f, "%.4f");
            ImGui::Dummy(ImVec2(0.0f, 3.0f));
            // ImGui::SliderFloat("Sun disk upperBound", &dynamicSkybox->upperBound, 0.00001f, 0.1f, "%.4f");
            // ImGui::SliderFloat("Sun disk lowerBound", &dynamicSkybox->lowerBound, 0.00001f, 0.1f, "%.4f");

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
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

                // Escalar muestras para que est�n m�s alineadas al centro del kernel
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




        //LTC�s
        struct LTC_matrices {
            GLuint mat1;
            GLuint mat2;
        };

        LTC_matrices mLTC;

        GLuint loadMTexture()
        {
            GLuint texture = 0;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64,
                0, GL_RGBA, GL_FLOAT, LTC1);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);
            return texture;
        }

        GLuint loadLUTTexture()
        {
            GLuint texture = 0;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64,
                0, GL_RGBA, GL_FLOAT, LTC2);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);
            return texture;
        }
    };
}