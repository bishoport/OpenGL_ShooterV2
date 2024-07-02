//#pragma once
//#include "../LibCoreHeaders.h"
//
//namespace libCore
//{
//    class DepthBufferPicking
//    {
//    public:
//        static DepthBufferPicking& getInstance() {
//            static DepthBufferPicking instance;
//            return instance;
//        }
//
//        float readDepthValueAtMousePosition(int mouseX, int mouseY, int screenWidth, int screenHeight, GLuint depthTexture) {
//            glBindTexture(GL_TEXTURE_2D, depthTexture);
//
//            int x = mouseX;
//            int y = screenHeight - mouseY; // Invertir el eje Y
//
//            float depth;
//            glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
//
//            glBindTexture(GL_TEXTURE_2D, 0);
//
//            std::cout << "Read depth at (" << x << ", " << y << "): " << depth << std::endl;
//
//            return depth;
//        }
//
//
//        glm::vec3 getWorldCoordinates(int mouseX, int mouseY, int screenWidth, int screenHeight, float depth, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
//            // Convertir coordenadas de pantalla a NDC
//            float ndcX = (2.0f * mouseX) / screenWidth - 1.0f;
//            float ndcY = 1.0f - (2.0f * mouseY) / screenHeight;  // Invertir Y
//            float ndcZ = 2.0f * depth - 1.0f;
//
//            glm::vec4 ndc(ndcX, ndcY, ndcZ, 1.0f);
//
//            // Convertir NDC a coordenadas de clip space
//            glm::vec4 clipSpace = glm::inverse(projectionMatrix) * ndc;
//            clipSpace /= clipSpace.w;
//
//            // Convertir de clip space a coordenadas de view space
//            glm::vec4 viewSpace = glm::inverse(viewMatrix) * clipSpace;
//
//            return glm::vec3(viewSpace);
//        }
//
//
//
//        void pickObject(Ref<Viewport> viewport, const std::vector<Ref<ModelContainer>>& modelsInScene) {
//            double mouseX;
//            double mouseY;
//            glfwGetCursorPos(libCore::EngineOpenGL::GetWindow(), &mouseX, &mouseY);
//
//            // Leer el valor del depth buffer
//            GLuint depthTexture = viewport->gBuffer->getTexture("depth");
//            float depth = readDepthValueAtMousePosition(static_cast<int>(mouseX), static_cast<int>(mouseY), viewport->viewportSize.x, viewport->viewportSize.y, depthTexture);
//
//            std::cout << "Depth value at mouse position: " << depth << std::endl;
//
//            if (depth == 1.0f) {
//                // Depth buffer value of 1.0 means no depth information (background)
//                std::cout << "No object found at mouse position." << std::endl;
//                return;
//            }
//
//            // Convertir a coordenadas de espacio de mundo
//            glm::vec3 worldCoords = getWorldCoordinates(static_cast<int>(mouseX), static_cast<int>(mouseY), viewport->viewportSize.x, viewport->viewportSize.y, depth, viewport->camera->projection, viewport->camera->view);
//
//            std::cout << "World Coordinates: (" << worldCoords.x << ", " << worldCoords.y << ", " << worldCoords.z << ")" << std::endl;
//
//            // Determinar qué objeto está en las coordenadas de espacio de mundo
//            bool objectPicked = false;
//            for (const auto& modelContainer : modelsInScene) {
//                for (const auto& model : modelContainer->models) {
//                    // Obtener la transformación del modelo
//                    glm::mat4 modelMatrix = model->transform.getMatrix();
//
//                    for (const auto& mesh : model->meshes) {
//                        // Transformar el punto a las coordenadas locales del modelo
//                        glm::vec3 localCoords = glm::vec3(glm::inverse(modelMatrix) * glm::vec4(worldCoords, 1.0f));
//
//                        if (mesh->IsPointInsideAABB(localCoords)) {
//                            std::cout << "Picked object: " << mesh->meshName << std::endl;
//                            objectPicked = true;
//                            break; // Rompe el bucle de meshes
//                        }
//                    }
//                    if (objectPicked) break; // Rompe el bucle de models
//                }
//                if (objectPicked) break; // Rompe el bucle de modelContainers
//            }
//        }
//
//    private:
//        DepthBufferPicking() {} // Constructor privado
//    };
//}
