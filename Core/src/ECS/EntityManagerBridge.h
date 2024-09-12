#pragma once

#include "EntityManager.h"  // Incluir la clase original de EntityManager

namespace libCore
{

    class EntityManagerBridge {
    public:
        // Crear una nueva entidad vac�a
        entt::entity CreateEntity(const std::string& name);

        // Crear una entidad desde un modelo
        entt::entity CreateEntityFromModel(const std::string& modelName);

        // Obtener el nombre de una entidad
        std::string GetEntityName(entt::entity entity);

        // Verificar si una entidad tiene un componente espec�fico
        template<typename T>
        bool HasComponent(entt::entity entity);

        // Destruir una entidad
        void DestroyEntity(entt::entity entity);

    };



}
