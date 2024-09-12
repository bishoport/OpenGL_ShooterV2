#pragma once

#include "EntityManager.h"  // Incluir la clase original de EntityManager
#include <tuple>

namespace libCore {

    class EntityManagerBridge {
    public:
        // Crear una nueva entidad vacía
        entt::entity CreateEntity(const std::string& name);

        // Crear una entidad desde un modelo
        entt::entity CreateEntityFromModel(const std::string& modelName);

        // Obtener el nombre de una entidad
        std::string GetEntityName(entt::entity entity);

        // Verificar si una entidad tiene un componente específico
        template<typename T>
        bool HasComponent(entt::entity entity);

        // Destruir una entidad
        void DestroyEntity(entt::entity entity);

        // Métodos para TransformComponent (posicion, rotación, escala)
        std::tuple<float, float, float> GetPosition(entt::entity entity);
        void SetPosition(entt::entity entity, float x, float y, float z);

        std::tuple<float, float, float> GetRotation(entt::entity entity);  // Rotación en Euler
        void SetRotation(entt::entity entity, float x, float y, float z);  // Establecer rotación en Euler

        std::tuple<float, float, float> GetScale(entt::entity entity);
        void SetScale(entt::entity entity, float x, float y, float z);
    };
}
