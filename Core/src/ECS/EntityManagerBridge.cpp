#include "EntityManagerBridge.h"
#include "../tools/AssetsManager.h"
#include "Components.h"  // Para los componentes como TagComponent



namespace libCore
{

    // Crear una nueva entidad vacía
    entt::entity EntityManagerBridge::CreateEntity(const std::string& name) {
        return EntityManager::GetInstance().CreateEmptyGameObject(name);
    }

    // Crear una entidad desde un modelo
    entt::entity EntityManagerBridge::CreateEntityFromModel(const std::string& modelName) {
        auto model = AssetsManager::GetInstance().GetModel(modelName);
        if (!model) {
            std::cerr << "Modelo no encontrado: " << modelName << std::endl;
            return entt::null;
        }
        return EntityManager::GetInstance().CreateGameObjectFromModel(model, entt::null);
    }

    // Obtener el nombre de una entidad
    std::string EntityManagerBridge::GetEntityName(entt::entity entity) {
        if (EntityManager::GetInstance().HasComponent<TagComponent>(entity)) {
            return EntityManager::GetInstance().GetComponent<TagComponent>(entity).Tag;
        }
        return "Unnamed";
    }

    // Verificar si una entidad tiene un componente específico
    template<typename T>
    bool EntityManagerBridge::HasComponent(entt::entity entity) {
        return EntityManager::GetInstance().HasComponent<T>(entity);
    }

    // Destruir una entidad
    void EntityManagerBridge::DestroyEntity(entt::entity entity) {
        EntityManager::GetInstance().DestroyEntity(entity);
    }

}
