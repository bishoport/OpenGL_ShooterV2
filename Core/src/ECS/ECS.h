#pragma once

#include "../Core/Transform.h"
#include "../Core/Material.h"
#include "../Core/Mesh.h"
#include "../Core/Light.hpp"
#include <entt.hpp>
#include <string>
#include <unordered_map>
#include <functional>
#include "../Core/Lights/DirectionalLight.h"
#include "../Core/AABB.h"
#include "../Core/UUID.h"
#include "../tools/Camera.h"

namespace libCore
{
    class Script {
    public:
        virtual ~Script() = default;

        // Métodos del ciclo de vida
        virtual void Init() {}
        virtual void Update(float deltaTime) {}

        // Métodos para establecer la entidad y el registro
        void SetEntity(entt::entity entity, Ref<entt::registry> registry) {
            m_Entity = entity;
            m_Registry = registry;
        }

        // Métodos para gestionar componentes
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

    struct IDComponent {
        UUID ID;
        bool markToDelete = false;

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
        IDComponent(UUID id)
            : ID(id) {}
    };

    struct TagComponent {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
            : Tag(tag) {}
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
        std::vector<glm::mat4> instanceMatrices;

        MeshComponent() = default;
        MeshComponent(const Ref<Mesh>& mesh) : mesh(mesh) {}
    };

    struct AABBComponent
    {
        Ref<AABB> aabb = CreateRef<AABB>(); 
    };

    struct MaterialComponent
    {
        Ref<Material> material;
    };

    struct TransformComponent {
        Ref<Transform> transform = CreateRef<Transform>();
        glm::mat4 accumulatedTransform = glm::mat4(1.0f); // Transformación acumulada

        glm::mat4 getGlobalTransform(entt::entity entity, entt::registry& registry) const {
            glm::mat4 globalTransform = transform->getLocalModelMatrix();
            if (registry.has<ParentComponent>(entity)) {
                entt::entity parentEntity = registry.get<ParentComponent>(entity).parent;
                if (registry.valid(parentEntity)) {
                    globalTransform = registry.get<TransformComponent>(parentEntity).getGlobalTransform(parentEntity, registry) * globalTransform;
                }
            }
            return globalTransform;
        }

        void setTransformFromGlobal(const glm::mat4& globalTransform, entt::entity entity, entt::registry& registry) {
            glm::mat4 parentGlobalTransform = glm::mat4(1.0f);
            if (registry.has<ParentComponent>(entity)) {
                entt::entity parentEntity = registry.get<ParentComponent>(entity).parent;
                if (registry.valid(parentEntity)) {
                    parentGlobalTransform = registry.get<TransformComponent>(parentEntity).getGlobalTransform(parentEntity, registry);
                }
            }
            glm::mat4 newLocalTransform = glm::inverse(parentGlobalTransform) * globalTransform;
            transform->setMatrix(newLocalTransform);
        }
    };

    struct CameraComponent
    {
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
        float FOVdeg = 45.0f;
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 cameraMatrix = glm::mat4(1.0f);

        int width = 800;
        int height = 600;
    };

    /*struct LightComponent
    {
        Ref<Light> light;
    };*/

    /*struct DirectionalLightComponent
    {
        Ref<DirectionalLight> directionalLight = CreateRef<DirectionalLight>();
    };*/

    struct ScriptComponent {

        Ref<Script> instance = nullptr;

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


    class ScriptFactory {
    public:
        using ScriptCreator = std::function<Ref<Script>()>;

        static ScriptFactory& GetInstance() {
            static ScriptFactory instance;
            return instance;
        }

        template<typename ScriptType>
        void RegisterScript(const std::string& scriptName) {
            creators[scriptName] = []() -> Ref<Script> {
                return CreateRef<ScriptType>();
            };
        }

        Ref<Script> CreateScript(const std::string& scriptName) {
            auto it = creators.find(scriptName);
            if (it != creators.end()) {
                return it->second();
            }
            return nullptr;
        }

        const std::unordered_map<std::string, ScriptCreator>& GetCreators() const {
            return creators;
        }

    private:
        std::unordered_map<std::string, ScriptCreator> creators;

        ScriptFactory() {}
        ScriptFactory(const ScriptFactory&) = delete;
        ScriptFactory& operator=(const ScriptFactory&) = delete;
    };
}


//#pragma once
//
//#include "../Core/Transform.h"
//#include "../Core/Material.h"
//#include "../Core/Mesh.h"
//#include "../Core/Light.hpp"
//#include <entt.hpp>
//
//namespace libCore
//{
//    class Script {
//    public:
//        virtual ~Script() = default;
//
//        // Métodos del ciclo de vida
//        virtual void Init() {}
//        virtual void Update(float deltaTime) {}
//
//        // Métodos para establecer la entidad y el registro
//        void SetEntity(entt::entity entity, Ref<entt::registry> registry) {
//            m_Entity = entity;
//            m_Registry = registry;
//        }
//
//        // Métodos para gestionar componentes
//        template<typename T>
//        T& GetComponent() {
//            return m_Registry->get<T>(m_Entity);
//        }
//
//        template<typename T>
//        bool HasComponent() {
//            return m_Registry->has<T>(m_Entity);
//        }
//
//        template<typename T, typename... Args>
//        T& AddComponent(Args&&... args) {
//            return m_Registry->emplace<T>(m_Entity, std::forward<Args>(args)...);
//        }
//
//    protected:
//        entt::entity m_Entity;
//        Ref<entt::registry> m_Registry;
//    };
//
//    struct TransformComponent
//    {
//        Ref<Transform> transform = CreateRef<Transform>();
//        glm::mat4 accumulatedTransform = glm::mat4(1.0f); // Transformación acumulada
//    };
//
//    struct ParentComponent {
//        entt::entity parent = entt::null;
//    };
//
//    struct ChildComponent {
//        std::vector<entt::entity> children;
//    };
//
//    struct MeshComponent
//    {
//        Ref<Mesh> mesh = CreateRef<Mesh>();
//    };
//
//    struct MaterialComponent 
//    {
//        Ref<Material> material = CreateRef<Material>();
//    };
//
//    struct LightComponent
//    {
//        Ref<Light> light;
//    };
//
//    struct DirectionalLightComponent
//    {
//        bool showDebug = false;
//        glm::vec3 color;
//        float intensity = 2.0f; // Añadido para controlar la intensidad de la luz
//
//        //--DIRECTIONAL LIGHT--------------------------------
//        glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
//        glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
//        glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
//
//        glm::vec3 direction = glm::vec3(0.0f, -10.0f, 0.0f);
//        float currentSceneRadius = 10.0f;
//        float sceneRadiusOffset = 10.0f;
//
//        // Shadow values
//        int shadowMapResolution = 1024;
//
//        glm::mat4 shadowMVP = glm::mat4(1.0f);
//        bool drawShadows = false;
//        float near_plane = 0.1f, far_plane = 100.0f;
//        float shadowIntensity = 0.5f;
//        bool usePoisonDisk = false;
//        float orthoLeft = -10.0f;
//        float orthoRight = 10.0f;
//        float orthoBottom = -10.0f;
//        float orthoTop = 10.0f;
//
//        float orthoNear = 0.1f;
//        float orthoNearOffset = 0.0f;
//        float orthoFar = 100.0f;
//        float orthoFarOffset = 0.0f;
//
//        float angleX = 0.0f;
//        float angleY = 0.0f;
//
//        glm::mat4 shadowBias = glm::mat4(
//            0.5, 0.0, 0.0, 0.0,
//            0.0, 0.5, 0.0, 0.0,
//            0.0, 0.0, 0.5, 0.0,
//            0.5, 0.5, 0.5, 1.0
//        );
//        //--SCENE BOUNDS
//        std::pair<glm::vec3, float> SceneBounds = { glm::vec3(0.0f), 30.0f };
//
//        //---------------------------------------------------
//    };
//
//
//    struct ScriptComponent {
//
//        Ref<Script> instance;
//
//        ScriptComponent() = default;
//        ScriptComponent(const ScriptComponent&) = default;
//        ScriptComponent(Ref<Script> script) : instance(script) {}
//
//        // Inicializar el script
//        void Init() {
//            if (instance) {
//                instance->Init();
//            }
//        }
//
//        // Actualizar el script
//        void Update(float deltaTime) {
//            if (instance) {
//                instance->Update(deltaTime);
//            }
//        }
//    };
//
//
//
//
//    class ScriptFactory {
//    public:
//        using ScriptCreator = std::function<Ref<IScript>()>;
//
//        static ScriptFactory& GetInstance() {
//            static ScriptFactory instance;
//            return instance;
//        }
//
//        template<typename ScriptType>
//        void RegisterScript(const std::string& scriptName) {
//            creators[scriptName] = []() -> Ref<IScript> {
//                return CreateRef<ScriptType>();
//            };
//        }
//
//        Ref<IScript> CreateScript(const std::string& scriptName) {
//            auto it = creators.find(scriptName);
//            if (it != creators.end()) {
//                return it->second();
//            }
//            return nullptr;
//        }
//
//        const std::unordered_map<std::string, ScriptCreator>& GetCreators() const {
//            return creators;
//        }
//
//    private:
//        std::unordered_map<std::string, ScriptCreator> creators;
//
//        ScriptFactory() {}
//        ScriptFactory(const ScriptFactory&) = delete;
//        ScriptFactory& operator=(const ScriptFactory&) = delete;
//    };
//}