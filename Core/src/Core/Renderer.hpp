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
#include "Itc_matrix_floats.hpp"
#include "../tools/LUTGenerator.hpp"
#include "../ECS/EntityManager.hpp"

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
        bool dynamicIBL = false;  // Cambia esto a `false` para IBL est�tico
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

        bool renderInPause = false;

        static Renderer& getInstance() {
            static Renderer instance;
            return instance;
        }



        void initialize() 
        {
            setupAxes();

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
            ibl->prepareIBL(640, 480,dynamicIBL);
            //-------------------------------------------------------

            

            //--SSAO
            setupQuad();// Configuraci�n del quad para SSAO
            ssaoKernel = generateSSAOKernel();         // Genera el kernel
            noiseTexture = generateSSAONoiseTexture(); // Genera textura de ruido SSAO
            //-------------------------------------------------------


            //--AREA LIGHT
            //mLTC.mat1 = LUTGenerator::GetInstance().createLTC1Texture(64);
            //mLTC.mat2 = LUTGenerator::GetInstance().createLTC2Texture(64);
            mLTC.mat1 = loadMTexture();
            mLTC.mat2 = loadLUTTexture();
            //-------------------------------------------------------
        }
        


        void PushDebugGroup(const std::string& name) {
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name.c_str());
        }

        void PopDebugGroup() {
            glPopDebugGroup();
        }



        void RenderViewport(const Ref<Viewport>& viewport, const Timestep& m_deltaTime) 
        { 
            if (renderInPause == true) return;


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

            // Actualizar la c�mara
            viewport->camera->Inputs(m_deltaTime);
            viewport->camera->updateMatrix();

            glEnable(GL_DEPTH_TEST); // Habilitar el test de profundidad





            //--------------------------------------------------------------------------------
            //-----------------------DIRECTIONAL LIGHT SHADOW PASS----------------------------
            //--------------------------------------------------------------------------------
            auto& directionalLight = LightsManager::GetInstance().GetDirectionalLight();
            if (directionalLight != nullptr && directionalLight->drawShadows) {
                PushDebugGroup("Directional Light Shadow Pass");

                // Configura la vista y proyecci�n desde la perspectiva de la luz
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
                glViewport(0, 0, static_cast<GLsizei>(viewport->viewportSize.x), static_cast<GLsizei>(viewport->viewportSize.y));
                glClear(GL_DEPTH_BUFFER_BIT);

                libCore::ShaderManager::Get("direct_light_depth_shadows")->use();
                libCore::ShaderManager::Get("direct_light_depth_shadows")->setMat4("shadowMVP", directionalLight->shadowMVP);

                // Draw the models in the scene
                EntityManager::GetInstance().DrawGameObjects("direct_light_depth_shadows");

                viewport->framebuffer_shadowmap->unbindFBO();
                PopDebugGroup();
            }
            glViewport(0, 0, static_cast<GLsizei>(viewport->viewportSize.x), static_cast<GLsizei>(viewport->viewportSize.y));
            //--------------------------------------------------------------------------------
            //--------------------------------------------------------------------------------





            //------------------------------------------------------------------------------------------------------
            //---------------------------------------------DEFERRED-------------------------------------------------
            //------------------------------------------------------------------------------------------------------

            // 1.1 Renderizado de geometr�a en el GBuffer
            PushDebugGroup("Deferred Geometry Pass");
            viewport->gBuffer->bindGBuffer();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

            EntityManager::GetInstance().DrawGameObjects("geometryPass");

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

            //ALL Lights
            LightsManager::GetInstance().SetLightDataInShader("lightingPass");

            //-DIRECTIONAL Light
            if (LightsManager::GetInstance().GetDirectionalLight() != nullptr)
            {
                libCore::ShaderManager::Get("lightingPass")->setMat4("lightSpaceMatrix", directionalLight->shadowMVP);
            }

            libCore::ShaderManager::Get("lightingPass")->setBool("useSSAO", ssaoEnabled);
            libCore::ShaderManager::Get("lightingPass")->setFloat("exposure", hdrExposure);
            libCore::ShaderManager::Get("lightingPass")->setFloat("ambientLight", ambientLight);
            libCore::ShaderManager::Get("lightingPass")->setVec3("viewPos", viewport->camera->Position);
            libCore::ShaderManager::Get("lightingPass")->setFloat("F0Factor", F0Factor);


            //-AREA LIGHT
            libCore::ShaderManager::Get("lightingPass")->setInt("LTC1", 6);
            libCore::ShaderManager::Get("lightingPass")->setInt("LTC2", 7);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, mLTC.mat1);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, mLTC.mat2);
            //----------------------------------------------------------------------------------------------


            //-IBL
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

                    // Utiliza el m�todo bindTexture del FBO
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
            glClear(GL_COLOR_BUFFER_BIT); // No se limpia aqu� el GL_DEPTH_BUFFER_BIT!!!!
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
            EntityManager::GetInstance().DrawABBGameObjectMeshComponent("debug");
            //------------------------------------------------------------------------------------------



            //GRID
            glEnable(GL_BLEND);//Empieza zona alpha
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            // Aseg�rate de que la c�mara est� viendo el grid correctamente
            libCore::ShaderManager::Get("grid")->use();
            libCore::ShaderManager::Get("grid")->setMat4("view", viewport->camera->view);
            libCore::ShaderManager::Get("grid")->setMat4("projection", viewport->camera->projection);

            // Opcionalmente, podr�as setear la matriz del modelo
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            libCore::ShaderManager::Get("grid")->setMat4("model", modelMatrix);

            // Opcionalmente, deshabilita el depth test para asegurarte de que se renderice en la parte superior
            glDisable(GL_DEPTH_TEST);

            // Renderizar los ejes en el origen
            renderAxes(viewport);

            // Renderiza el grid
            renderGrid();
            
            

            


            // Habilita nuevamente el depth test para el resto de la escena
            glEnable(GL_DEPTH_TEST);
            //------------------------------------------------------------------------------------------
 
            
            

            // PASADA DE TEXTOS
            glm::mat4 model = glm::mat4(1.0f);
            libCore::ShaderManager::Get("text")->use();
            libCore::ShaderManager::Get("text")->setMat4("projection", viewport->camera->projection);
            libCore::ShaderManager::Get("text")->setMat4("model", model);
            libCore::ShaderManager::Get("text")->setMat4("view", viewport->camera->view);

            //--> Render Textos AQUI!!!
            glDisable(GL_BLEND); 
            glBlendFunc(GL_ONE, GL_ZERO);//Acaba zona alpha
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
            



            //GRID
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::Separator();
            ImGui::Text("Grid Settings");

            // Control para el tama�o del grid (n�mero de celdas)
            ImGui::SliderInt("Grid Size", &gridSize, 1, 100);

            // Control para el color del grid
            ImGui::ColorEdit3("Grid Color", glm::value_ptr(gridColor));

            // Control para la transparencia del grid
            ImGui::SliderFloat("Grid Transparency", &gridTransparency, 0.0f, 1.0f, "%.2f");

            // Control para el tama�o de la celda del grid
            ImGui::SliderFloat("Grid Cell Size", &gridCellSize, 0.1f, 10.0f, "%.2f");

            // Control para el grosor de las l�neas del grid
            ImGui::SliderFloat("Grid Line Thickness", &gridCellLineSize, 0.1f, 5.0f, "%.2f");

            ImGui::End();
        }


        void ShowAxisControlGUI()
        {
            ImGui::Begin("Axis Controls");

            ImGui::ColorEdit3("X Axis Color", glm::value_ptr(axisXColor));
            ImGui::ColorEdit3("Y Axis Color", glm::value_ptr(axisYColor));
            ImGui::ColorEdit3("Z Axis Color", glm::value_ptr(axisZColor));

            ImGui::SliderFloat("Axis Size", &axisSize, 0.01f, 0.5f); // Ajusta el tama�o de los ejes

            ImGui::SliderFloat2("Axis Position", glm::value_ptr(axisPosition), 0.0f, 1.0f); // Ajusta la posici�n en pantalla

            ImGui::End();
        }


    private:
        Renderer() {} // Constructor privado

        std::vector<glm::vec3> ssaoKernel;
        GLuint noiseTexture;
        const unsigned int NR_LIGHTS = 1;
        GLuint quadVAO, quadVBO;

        //WORLD GRID
        //GLuint gridVAO = 0;
        //GLuint gridVBO = 0;
        int gridSize = 10;
        glm::vec3 gridColor =  glm::vec3(0.1f, 0.1f, 0.1f);
        float gridTransparency = 1.0f;
        float gridCellSize = 1.0f;
        float gridCellLineSize = 1.0f;

        //WORLD AXIS
        glm::vec3 axisXColor = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 axisYColor = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 axisZColor = glm::vec3(0.0f, 0.0f, 1.0f);
        float axisSize = 0.1f; // Tama�o relativo de los ejes
        glm::vec2 axisPosition = glm::vec2(0.1f, 0.9f); // Posici�n relativa de los ejes (esquina superior izquierda)
        GLuint axesVAO;
        GLuint axesVBO;


        //--QUAD
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
        //--------------------------------------------------------------------------

        //--GRID
        void renderGrid()
        {
            static GLuint gridVAO = 0;
            static GLuint gridVBO = 0;

            // Si hay cambios en el tama�o del grid o el tama�o de las celdas, recrear el grid
            static int lastGridSize = gridSize;
            static float lastGridCellSize = gridCellSize;

            if (gridVAO == 0 || gridSize != lastGridSize || gridCellSize != lastGridCellSize)
            {
                lastGridSize = gridSize;
                lastGridCellSize = gridCellSize;

                // Generar las l�neas para el grid
                std::vector<float> vertices;

                for (int i = -gridSize; i <= gridSize; ++i)
                {
                    float position = i * gridCellSize;

                    // L�neas en la direcci�n X (horizontales)
                    vertices.push_back(-gridSize * gridCellSize);
                    vertices.push_back(0.0f);
                    vertices.push_back(position);

                    vertices.push_back(gridSize * gridCellSize);
                    vertices.push_back(0.0f);
                    vertices.push_back(position);

                    // L�neas en la direcci�n Z (verticales)
                    vertices.push_back(position);
                    vertices.push_back(0.0f);
                    vertices.push_back(-gridSize * gridCellSize);

                    vertices.push_back(position);
                    vertices.push_back(0.0f);
                    vertices.push_back(gridSize * gridCellSize);
                }

                if (gridVAO == 0)
                {
                    glGenVertexArrays(1, &gridVAO);
                    glGenBuffers(1, &gridVBO);
                }

                glBindVertexArray(gridVAO);

                glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

                // Posiciones de v�rtices
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

                glBindVertexArray(0);
            }

            // Configurar las propiedades del shader antes de renderizar
            glLineWidth(gridCellLineSize);
            libCore::ShaderManager::Get("grid")->use();
            libCore::ShaderManager::Get("grid")->setVec3("gridColor", gridColor);
            libCore::ShaderManager::Get("grid")->setFloat("gridTransparency", gridTransparency);

            // Renderizar el grid
            glBindVertexArray(gridVAO);
            glDrawArrays(GL_LINES, 0, (gridSize * 2 + 1) * 4); // Cantidad de v�rtices en el grid
            glBindVertexArray(0);
        }
        //--------------------------------------------------------------------------
        
        //--WORLD_AXIS
        void setupAxes()
        {
            // Definir los v�rtices de las l�neas de los ejes X, Y, Z
            float axesVertices[] = {
                // Eje X (rojo)
                0.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,

                // Eje Y (verde)
                0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,

                // Eje Z (azul)
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f,
            };

            glGenVertexArrays(1, &axesVAO);
            glGenBuffers(1, &axesVBO);

            glBindVertexArray(axesVAO);

            glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(axesVertices), axesVertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            glBindVertexArray(0);
        }
        //void renderAxes()
        //{
        //    glBindVertexArray(axesVAO);

        //    glLineWidth(gridCellLineSize);

        //    // Eje X (rojo)
        //    libCore::ShaderManager::Get("axes")->use();
        //    libCore::ShaderManager::Get("axes")->setVec3("axisColor", glm::vec3(1.0f, 0.0f, 0.0f)); // Rojo para X
        //    glDrawArrays(GL_LINES, 0, 2);

        //    // Eje Y (verde)
        //    libCore::ShaderManager::Get("axes")->setVec3("axisColor", glm::vec3(0.0f, 1.0f, 0.0f)); // Verde para Y
        //    glDrawArrays(GL_LINES, 2, 2);

        //    // Eje Z (azul)
        //    libCore::ShaderManager::Get("axes")->setVec3("axisColor", glm::vec3(0.0f, 0.0f, 1.0f)); // Azul para Z
        //    glDrawArrays(GL_LINES, 4, 2);

        //    glBindVertexArray(0);
        //}
        void renderAxes(const Ref<Viewport>& viewport)
        {
            glBindVertexArray(axesVAO);

            glLineWidth(2.0f); // Aseg�rate de que la l�nea sea lo suficientemente gruesa para ser visible

            libCore::ShaderManager::Get("axes")->use();
            libCore::ShaderManager::Get("axes")->setMat4("view", viewport->camera->view);
            libCore::ShaderManager::Get("axes")->setMat4("projection", viewport->camera->projection);

            // Eje X (rojo)
            libCore::ShaderManager::Get("axes")->setVec3("axisColor", glm::vec3(1.0f, 0.0f, 0.0f)); // Rojo
            glDrawArrays(GL_LINES, 0, 2);

            // Eje Y (verde)
            libCore::ShaderManager::Get("axes")->setVec3("axisColor", glm::vec3(0.0f, 1.0f, 0.0f)); // Verde
            glDrawArrays(GL_LINES, 2, 2);

            // Eje Z (azul)
            libCore::ShaderManager::Get("axes")->setVec3("axisColor", glm::vec3(0.0f, 0.0f, 1.0f)); // Azul
            glDrawArrays(GL_LINES, 4, 2);

            glBindVertexArray(0);
        }


        //--------------------------------------------------------------------------



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

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, LTC1);

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

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, LTC2);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);
            return texture;
        }
    };
}