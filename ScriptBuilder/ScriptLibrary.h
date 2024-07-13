#pragma once

#ifdef SCRIPT_LIBRARY_EXPORTS
#define SCRIPT_LIBRARY_API __declspec(dllexport)
#else
#define SCRIPT_LIBRARY_API __declspec(dllimport)
#endif


#include <memory>
#include <ECS/ECS.h>

namespace libCore {

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    class SCRIPT_LIBRARY_API Script {
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

    class SCRIPT_LIBRARY_API ExampleScript : public Script {
    public:
        void Init() override {
            // Inicialización del script
        }

        void Update(float deltaTime) override {
            // Lógica de actualización del script
            if (HasComponent<TransformComponent>()) {
                auto& transform = GetComponent<TransformComponent>().transform;
                // Modificar la posición como ejemplo
                transform->position.x += 0.01f * deltaTime;
            }
        }
    };

    // Función para crear una instancia de ExampleScript
    extern "C" SCRIPT_LIBRARY_API Script * CreateExampleScript();
}
