#pragma once

#include "../LibCoreHeaders.h"
#include "../ECS/ECS.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace libCore
{
    enum LightType {
        POINT,
        SPOT,
        AREA
    };

    class Light
    {
    public:
        int id = 0;
        LightType type;
        bool showDebug = false;

        libCore::Transform transform;

        glm::vec3 color;
        float intensity = 2.0f; // Añadido para controlar la intensidad de la luz

        // Default attenuation parameters and calculate radius
        float Linear = 0.07f;
        float Quadratic = 0.01f;
        float Radius = 90.0f;
        float LightSmoothness = 2.8f;

        // Propiedades específicas para SpotLight
        float innerCutoff = glm::cos(glm::radians(12.5f));
        float outerCutoff = glm::cos(glm::radians(17.5f));

        // Propiedades específicas para AreaLight
        std::vector<glm::vec3> areaLightpoints;
        bool twoSided = true;

        float LUT_SIZE = 64.0f;
        float LUT_SCALE = (64.0f - 1.0f) / 64.0f;
        float LUT_BIAS = 0.5f / 64.0f;

        Light(LightType lightType) : type(lightType), color(1.0f, 1.0f, 1.0f), intensity(1.0f)
        {
            // Inicializa puntos con un cuadrado por defecto
            areaLightpoints = {
                glm::vec3( 2.5f, 0.0f, 14.7f),
                glm::vec3(-2.5f, 0.0f, 14.7f),
                glm::vec3(-2.5f, 0.0f, -14.7f),
                glm::vec3( 2.5f, 0.0f, -14.7f)
            };

            InitializeBuffers();
            UpdateVertices();
        }

        ~Light() {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
        }

        //--METHODS
        void SetLightDataToShader(const std::string& shader)
        {
            glm::vec3 scaledColor = color * intensity; // Escalar el color de la luz según la intensidad

            if (type == POINT)
            {
                libCore::ShaderManager::Get(shader)->setVec3("pointLights[" + std::to_string(id) + "].Position", transform.position);
                libCore::ShaderManager::Get(shader)->setVec3("pointLights[" + std::to_string(id) + "].colorAmbient", scaledColor); // Usar color escalado
                libCore::ShaderManager::Get(shader)->setFloat("pointLights[" + std::to_string(id) + "].Linear", Linear);
                libCore::ShaderManager::Get(shader)->setFloat("pointLights[" + std::to_string(id) + "].Quadratic", Quadratic);
                libCore::ShaderManager::Get(shader)->setFloat("pointLights[" + std::to_string(id) + "].Radius", Radius);
                libCore::ShaderManager::Get(shader)->setFloat("pointLights[" + std::to_string(id) + "].LightSmoothness", LightSmoothness);
            }
            else if (type == SPOT)
            {
                libCore::ShaderManager::Get(shader)->setVec3("spotLights[" + std::to_string(id) + "].Position", transform.position);
                libCore::ShaderManager::Get(shader)->setVec3("spotLights[" + std::to_string(id) + "].colorAmbient", scaledColor); // Usar color escalado
                libCore::ShaderManager::Get(shader)->setFloat("spotLights[" + std::to_string(id) + "].Linear", Linear);
                libCore::ShaderManager::Get(shader)->setFloat("spotLights[" + std::to_string(id) + "].Quadratic", Quadratic);
                libCore::ShaderManager::Get(shader)->setFloat("spotLights[" + std::to_string(id) + "].Radius", Radius);
                libCore::ShaderManager::Get(shader)->setFloat("spotLights[" + std::to_string(id) + "].LightSmoothness", LightSmoothness);
                libCore::ShaderManager::Get(shader)->setVec3("spotLights[" + std::to_string(id) + "].Direction", transform.getRotationMatrix() * glm::vec4(0, 0, -1, 0));
                libCore::ShaderManager::Get(shader)->setFloat("spotLights[" + std::to_string(id) + "].innerCutoff", innerCutoff);
                libCore::ShaderManager::Get(shader)->setFloat("spotLights[" + std::to_string(id) + "].outerCutoff", outerCutoff);
            }
            else if (type == AREA)
            {
                libCore::ShaderManager::Get(shader)->setVec3("areaLights[" + std::to_string(id) + "].color", scaledColor); // Usar color escalado
                libCore::ShaderManager::Get(shader)->setFloat("areaLights[" + std::to_string(id) + "].intensity", intensity);

                for (size_t i = 0; i < areaLightpoints.size(); ++i)
                {
                    glm::vec3 transformedPoint = transform.position + areaLightpoints[i];
                    libCore::ShaderManager::Get(shader)->setVec3("areaLights[" + std::to_string(id) + "].points[" + std::to_string(i) + "]", transformedPoint);
                }
                libCore::ShaderManager::Get(shader)->setBool("areaLights[" + std::to_string(id) + "].twoSided", twoSided);
            }

            // Enviar uniformes adicionales
            libCore::ShaderManager::Get(shader)->setFloat("LUT_SIZE", LUT_SIZE);
            libCore::ShaderManager::Get(shader)->setFloat("LUT_SCALE", LUT_SCALE);
            libCore::ShaderManager::Get(shader)->setFloat("LUT_BIAS", LUT_BIAS);
        }



        void DrawDebugLight(const std::string& shader)
        {
            if (!showDebug) return;


            glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position);

            libCore::ShaderManager::Get(shader)->setVec4("u_Color", glm::vec4(color, 1.0f));
            libCore::ShaderManager::Get(shader)->setMat4("model", model);

            glBindVertexArray(VAO);

            glPointSize(30.0f);

            if (type == POINT || type == SPOT)
            {
                // Dibujar un solo punto
                glDrawArrays(GL_POINTS, 0, 1);
            }
            else if (type == AREA)
            {
                // Dibujar los puntos del área
                glDrawArrays(GL_POINTS, 0, vertices.size());

                // Dibujar líneas entre los puntos para formar el polígono
                std::vector<GLuint> indices;
                for (size_t i = 0; i < vertices.size(); ++i)
                {
                    indices.push_back(i);
                    indices.push_back((i + 1) % vertices.size());
                }

                GLuint EBO;
                glGenBuffers(1, &EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);

                glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glDeleteBuffers(1, &EBO);
            }

            glBindVertexArray(0);
        }


        void UpdateVertices()
        {
            vertices.clear();

            if (type == POINT || type == SPOT)
            {
                vertices.push_back(glm::vec3(0.0f)); // Usar (0,0,0) ya que el modelo se encargará del desplazamiento
            }
            else if (type == AREA)
            {
                for (const auto& point : areaLightpoints)
                {
                    vertices.push_back(point); // Solo agregar los puntos del área sin transformarlos aquí
                }
            }

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

    private:
        GLuint VAO, VBO;
        std::vector<glm::vec3> vertices;

        void InitializeBuffers()
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 100, nullptr, GL_DYNAMIC_DRAW); // Prealoca espacio para 100 vértices

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    };
}