#include "Scene.h"
#include "EntityManager.hpp"
#include <fstream>
#include "../Core/Transform.h"
#include "../Core/Material.h"
#include "SerializatorManager.h"

namespace libCore 
{
    Scene::Scene() {}

    //--PLAY LIFE CYCLE
    void Scene::Init() 
    {
        EntityManager::GetInstance().InitScripts();
    }
    void Scene::Update(Timestep deltaTime) 
    {
        EntityManager::GetInstance().UpdateScripts(deltaTime);
    }
    void Scene::Render(Timestep m_deltaTime) {}
    //---------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------





    void Scene::SerializeScene() {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        auto& entityManager = EntityManager::GetInstance();

        entityManager.m_registry->each([&](auto entity) {
            out << YAML::BeginMap; // Entity
            out << YAML::Key << "Entity" << YAML::Value << (uint32_t)entity; // Serialize entity ID

            // Serialize components
            if (entityManager.HasComponent<TransformComponent>(entity)) {
                out << YAML::Key << "TransformComponent";
                out << YAML::Value << SerializeTransformComponent(entityManager.GetComponent<TransformComponent>(entity));
            }

            if (entityManager.HasComponent<MaterialComponent>(entity)) {
                out << YAML::Key << "MaterialComponent";
                out << YAML::Value << SerializeMaterialComponent(entityManager.GetComponent<MaterialComponent>(entity));
            }

            out << YAML::EndMap; // Entity
            });

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout("assets/Scenes/" + sceneName + ".yaml");
        fout << out.c_str();
    }
    void Scene::DeserializeScene() {
        YAML::Node data = YAML::LoadFile("assets/Scenes/" + sceneName + ".yaml");
        if (!data["Scene"]) {
            return;
        }

        auto& entityManager = EntityManager::GetInstance();
        auto entities = data["Entities"];
        if (entities) {
            for (auto entityNode : entities) {
                uint32_t entityID = entityNode["Entity"].as<uint32_t>();
                entt::entity entity = (entt::entity)entityID;

                if (entityNode["TransformComponent"]) {
                    auto component = DeserializeTransformComponent(entityNode["TransformComponent"]);
                    entityManager.m_registry->emplace_or_replace<TransformComponent>(entity, component);
                }

                if (entityNode["MaterialComponent"]) {
                    auto component = DeserializeMaterialComponent(entityNode["MaterialComponent"]);
                    entityManager.m_registry->emplace_or_replace<MaterialComponent>(entity, component);
                }
            }
        }
    }
}
