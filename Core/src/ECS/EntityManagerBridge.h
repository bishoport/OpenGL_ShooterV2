#pragma once

#include "EntityManager.h"  // Incluir la clase original de EntityManager
#include <tuple>

namespace libCore {

    class EntityManagerBridge {
    public:

        //--ENTITES
        entt::entity CreateEntity(const std::string& name);
        entt::entity CreateEntityFromModel(const std::string& modelName);
        void DestroyEntity(entt::entity entity);
        std::string GetEntityName(entt::entity entity);
        //----------------------------------------------------------------------------------------

        //--CHECK COMPONENTS
        template<typename T>
        bool HasComponent(entt::entity entity);
        //----------------------------------------------------------------------------------------

        //--TRANSFORM COMPONENT
        std::tuple<float, float, float> GetPosition(entt::entity entity);
        void SetPosition(entt::entity entity, float x, float y, float z);
        std::tuple<float, float, float> GetRotation(entt::entity entity);  // Rotaci�n en Euler
        void SetRotation(entt::entity entity, float x, float y, float z);  // Establecer rotaci�n en Euler
        std::tuple<float, float, float> GetScale(entt::entity entity);
        void SetScale(entt::entity entity, float x, float y, float z);
        //----------------------------------------------------------------------------------------
    };
}
