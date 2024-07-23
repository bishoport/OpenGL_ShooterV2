#pragma once

#include "../../LibCoreHeaders.h"

namespace libCore
{
	class DirectionalLight
	{
	public:

        bool showDebug = false;
        glm::vec3 color;
        float intensity = 2.0f; // Añadido para controlar la intensidad de la luz

        //--DIRECTIONAL LIGHT--------------------------------
        glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);

        glm::vec3 direction = glm::vec3(0.0f, -10.0f, 0.0f);
        float currentSceneRadius = 10.0f;
        float sceneRadiusOffset = 10.0f;

        // Shadow values
        int shadowMapResolution = 1024;

        glm::mat4 shadowMVP = glm::mat4(1.0f);
        bool drawShadows = false;
        float near_plane = 0.1f, far_plane = 100.0f;
        float shadowIntensity = 0.5f;
        bool usePoisonDisk = false;
        float orthoLeft  = -10.0f;
        float orthoRight = 10.0f;
        float orthoBottom = -10.0f;
        float orthoTop = 10.0f;

        float orthoNear = 0.1f;
        float orthoNearOffset = 0.0f;
        float orthoFar = 100.0f;
        float orthoFarOffset = 0.0f;

        float angleX = 0.0f;
        float angleY = 0.0f;

        glm::mat4 shadowBias = glm::mat4(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );
        //--SCENE BOUNDS
        std::pair<glm::vec3, float> SceneBounds = { glm::vec3(0.0f), 30.0f };


    private:
        //--DEBUG
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


    public:
        //---------------------------------------------------
        //---FUNCTIONS---------------------------------------
        //---------------------------------------------------

        void SetLightDataToShader(const std::string& shader)
        {
            glm::vec3 scaledColor = color * intensity; // Escalar el color de la luz según la intensidad

            libCore::ShaderManager::Get(shader)->setBool("directionalLight.isActive", true);
            libCore::ShaderManager::Get(shader)->setVec3("directionalLight.direction", direction);
            libCore::ShaderManager::Get(shader)->setVec3("directionalLight.ambient", ambient * intensity);
            libCore::ShaderManager::Get(shader)->setVec3("directionalLight.diffuse", diffuse * intensity);
            libCore::ShaderManager::Get(shader)->setVec3("directionalLight.specular", specular * intensity);

            // Enviar datos de sombras
            libCore::ShaderManager::Get(shader)->setBool("directionalLight.drawShadows", drawShadows);
            libCore::ShaderManager::Get(shader)->setFloat("directionalLight.shadowIntensity", shadowIntensity);
            libCore::ShaderManager::Get(shader)->setBool("directionalLight.usePoisonDisk", usePoisonDisk);
            libCore::ShaderManager::Get(shader)->setMat4("directionalLight.shadowBiasMVP", shadowBias * shadowMVP);
        } 
	};
}