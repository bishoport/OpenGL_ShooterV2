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
        bool renderable = true; //<- no está serializada, hay que agregarla

        Ref<Mesh> mesh = CreateRef<Mesh>();
        std::vector<glm::mat4> instanceMatrices;
        Ref<Model> originalModel = nullptr; // Referencia al modelo original
        bool isInstance = false; // Indica si es una instancia

        MeshComponent() = default;
        MeshComponent(const Ref<Mesh>& mesh, const Ref<Model>& originalModel = nullptr, bool isInstance = false)
            : mesh(mesh), originalModel(originalModel), isInstance(isInstance) {}
    };

    struct AABBComponent
    {
        Ref<AABB> aabb = CreateRef<AABB>(); 
    };

    struct MaterialComponent
    {
       // bool overrideValues = false;
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
                    globalTransform = registry.get<TransformComponent>(parentEntity).accumulatedTransform * globalTransform;
                }
            }
            return globalTransform;
        }

        void setTransformFromGlobal(const glm::mat4& globalTransform, entt::entity entity, entt::registry& registry) {
            glm::mat4 parentGlobalTransform = glm::mat4(1.0f);
            if (registry.has<ParentComponent>(entity)) {
                entt::entity parentEntity = registry.get<ParentComponent>(entity).parent;
                if (registry.valid(parentEntity)) {
                    parentGlobalTransform = registry.get<TransformComponent>(parentEntity).accumulatedTransform;
                }
            }
            glm::mat4 newLocalTransform = glm::inverse(parentGlobalTransform) * globalTransform;
            transform->setMatrix(newLocalTransform);
        }
    };



    struct CameraComponent
    {
        Ref<Camera> camera = nullptr;
    };


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