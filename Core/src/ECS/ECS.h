#pragma once

#include "../Core/Transform.h"
#include "../Core/Material.h"
#include "../Core/Mesh.h"
#include "../Core/Light.hpp"
#include <entt.hpp>

namespace libCore
{
    class Script {
    public:
        virtual ~Script() = default;

        // M�todos del ciclo de vida
        virtual void Init() {}
        virtual void Update(float deltaTime) {}

        // M�todos para establecer la entidad y el registro
        void SetEntity(entt::entity entity, Ref<entt::registry> registry) {
            m_Entity = entity;
            m_Registry = registry;
        }

        // M�todos para gestionar componentes
        template<typename T>
        T& GetComponent() {
            return m_Registry->get<T>(m_Entity);
        }

        template<typename T>
        bool HasComponent() {
            return m_Registry->has<T>(m_Entity);
        }

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            return m_Registry->emplace<T>(m_Entity, std::forward<Args>(args)...);
        }

    protected:
        entt::entity m_Entity;
        Ref<entt::registry> m_Registry;
    };

    struct TransformComponent
    {
        Ref<Transform> transform = CreateRef<Transform>();
        glm::mat4 accumulatedTransform = glm::mat4(1.0f); // Transformaci�n acumulada
    };

    struct ParentComponent {
        entt::entity parent = entt::null;
    };

    struct ChildComponent {
        std::vector<entt::entity> children;
    };

    struct MeshComponent
    {
        Ref<Mesh> mesh = CreateRef<Mesh>();
    };

    struct MaterialComponent 
    {
        Ref<Material> material = CreateRef<Material>();
    };

    struct LightComponent
    {
        Ref<Light> light;
    };

    struct DirectionalLightComponent
    {
        bool showDebug = false;
        glm::vec3 color;
        float intensity = 2.0f; // A�adido para controlar la intensidad de la luz

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
        float orthoLeft = -10.0f;
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

        //---------------------------------------------------
    };


    struct ScriptComponent {

        Ref<Script> instance;

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent&) = default;
        ScriptComponent(Ref<Script> script) : instance(script) {}

        // Inicializar el script
        void Init() {
            if (instance) {
                instance->Init();
            }
        }

        // Actualizar el script
        void Update(float deltaTime) {
            if (instance) {
                instance->Update(deltaTime);
            }
        }
    };
}