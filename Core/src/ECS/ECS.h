#pragma once

#include "../Core/Transform.h"
#include "../Core/Material.h"
#include "../Core/Mesh.h"
#include <entt.hpp>

//#include "ScriptLibrary.h"

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
        Ref<Transform> transform;
    };

    struct ParentComponent {
        entt::entity parent = entt::null;
    };

    struct ChildComponent {
        std::vector<entt::entity> children;
    };

    struct MeshComponent
    {
        Ref<Mesh> mesh;
    };


    struct MaterialComponent 
    {
        Ref<Material> material;
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






    //class ExampleScript : public Script {
    //public:
    //    void Init() override {
    //        // Inicializaci�n del script
    //    }

    //    void Update(float deltaTime) override {
    //        // L�gica de actualizaci�n del script
    //        if (m_Registry && m_Registry->has<TransformComponent>(m_Entity)) {
    //            //auto& transform = m_Registry->get<TransformComponent>(m_Entity).transform;
    //            auto& transform = GetComponent<TransformComponent>().transform;
    //            // Modificar la posici�n como ejemplo
    //            transform->position.x += 0.01f * deltaTime;
    //        }
    //    }
    //};
}