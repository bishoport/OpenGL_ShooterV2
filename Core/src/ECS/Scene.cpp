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

        // Serializar los modelos cargados en memoria
        out << YAML::Key << "Models" << YAML::Value << SerializeAllModels();

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

            if (entityManager.HasComponent<MeshComponent>(entity)) {
                out << YAML::Key << "MeshComponent";
                out << YAML::Value << SerializeMeshComponent(entityManager.GetComponent<MeshComponent>(entity));
            }

            if (entityManager.HasComponent<AABBComponent>(entity)) {
                out << YAML::Key << "AABBComponent";
                out << YAML::Value << SerializeAABBComponent(entityManager.GetComponent<AABBComponent>(entity));
            }

            if (entityManager.HasComponent<IDComponent>(entity)) {
                out << YAML::Key << "IDComponent";
                out << YAML::Value << SerializeIDComponent(entityManager.GetComponent<IDComponent>(entity));
            }

            if (entityManager.HasComponent<TagComponent>(entity)) {
                out << YAML::Key << "TagComponent";
                out << YAML::Value << SerializeTagComponent(entityManager.GetComponent<TagComponent>(entity));
            }

            if (entityManager.HasComponent<ScriptComponent>(entity)) {
                out << YAML::Key << "ScriptComponent";
                out << YAML::Value << SerializeScriptComponent(entityManager.GetComponent<ScriptComponent>(entity));
            }

            // Serializar el ParentComponent
            if (entityManager.HasComponent<ParentComponent>(entity)) {
                out << YAML::Key << "ParentComponent";
                auto& parentComponent = entityManager.GetComponent<ParentComponent>(entity);
                out << YAML::BeginMap;
                out << YAML::Key << "Parent" << YAML::Value << (uint32_t)parentComponent.parent;
                out << YAML::EndMap;
            }

            // Serializar el ChildComponent
            if (entityManager.HasComponent<ChildComponent>(entity)) {
                out << YAML::Key << "ChildComponent";
                auto& childComponent = entityManager.GetComponent<ChildComponent>(entity);
                out << YAML::BeginSeq;
                for (auto child : childComponent.children) {
                    out << (uint32_t)child;
                }
                out << YAML::EndSeq;
            }

            out << YAML::EndMap; // Entity
            });

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout("assets/Scenes/" + sceneName + ".yaml");
        fout << out.c_str();
    }


    //void Scene::SerializeScene() {
    //    YAML::Emitter out;
    //    out << YAML::BeginMap;
    //    out << YAML::Key << "Scene" << YAML::Value << "Untitled";

    //    // Serializar los modelos cargados en memoria
    //    out << YAML::Key << "Models" << YAML::Value << SerializeAllModels();

    //    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

    //    auto& entityManager = EntityManager::GetInstance();

    //    entityManager.m_registry->each([&](auto entity) {
    //        out << YAML::BeginMap; // Entity
    //        out << YAML::Key << "Entity" << YAML::Value << (uint32_t)entity; // Serialize entity ID

    //        // Serialize components
    //        if (entityManager.HasComponent<TransformComponent>(entity)) {
    //            out << YAML::Key << "TransformComponent";
    //            out << YAML::Value << SerializeTransformComponent(entityManager.GetComponent<TransformComponent>(entity));
    //        }

    //        if (entityManager.HasComponent<MaterialComponent>(entity)) {
    //            out << YAML::Key << "MaterialComponent";
    //            out << YAML::Value << SerializeMaterialComponent(entityManager.GetComponent<MaterialComponent>(entity));
    //        }

    //        if (entityManager.HasComponent<MeshComponent>(entity)) {
    //            out << YAML::Key << "MeshComponent";
    //            out << YAML::Value << SerializeMeshComponent(entityManager.GetComponent<MeshComponent>(entity));
    //        }

    //        if (entityManager.HasComponent<AABBComponent>(entity)) {
    //            out << YAML::Key << "AABBComponent";
    //            out << YAML::Value << SerializeAABBComponent(entityManager.GetComponent<AABBComponent>(entity));
    //        }

    //        /*if (entityManager.HasComponent<CameraComponent>(entity)) {
    //            out << YAML::Key << "CameraComponent";
    //            out << YAML::Value << SerializeCameraComponent(entityManager.GetComponent<CameraComponent>(entity));
    //        }*/

    //        if (entityManager.HasComponent<IDComponent>(entity)) {
    //            out << YAML::Key << "IDComponent";
    //            out << YAML::Value << SerializeIDComponent(entityManager.GetComponent<IDComponent>(entity));
    //        }

    //        if (entityManager.HasComponent<TagComponent>(entity)) {
    //            out << YAML::Key << "TagComponent";
    //            out << YAML::Value << SerializeTagComponent(entityManager.GetComponent<TagComponent>(entity));
    //        }

    //        if (entityManager.HasComponent<ScriptComponent>(entity)) {
    //            out << YAML::Key << "ScriptComponent";
    //            out << YAML::Value << SerializeScriptComponent(entityManager.GetComponent<ScriptComponent>(entity));
    //        }

    //        out << YAML::EndMap; // Entity
    //        });

    //    out << YAML::EndSeq;
    //    out << YAML::EndMap;

    //    std::ofstream fout("assets/Scenes/" + sceneName + ".yaml");
    //    fout << out.c_str();
    //}


    void Scene::DeserializeScene() {
        YAML::Node data = YAML::LoadFile("assets/Scenes/" + sceneName + ".yaml");
        if (!data["Scene"]) {
            return;
        }

        // Deserializar los modelos cargados en memoria
        if (data["Models"]) {
            DeserializeAllModels(data["Models"]);
        }

        auto& entityManager = EntityManager::GetInstance();
        auto entities = data["Entities"];

        // Mapa para mantener la correspondencia entre los IDs de las entidades y las entidades deserializadas
        std::unordered_map<uint32_t, entt::entity> entityMap;

        if (entities) {
            // Primer pase: Crear todas las entidades y deserializar los componentes básicos
            for (auto entityNode : entities) {
                uint32_t entityID = entityNode["Entity"].as<uint32_t>();

                entt::entity entity;

                if (entityNode["MeshComponent"]) {
                    std::string meshModelName = entityNode["MeshComponent"]["MeshName"].as<std::string>();
                    Ref<Model> model = AssetsManager::GetInstance().GetModelByMeshName(meshModelName);
                    if (model != nullptr) {
                        entity = entityManager.CreateGameObjectFromModel(model, entt::null);

                        // Deserializa y asigna componentes
                        if (entityNode["TransformComponent"]) {
                            auto component = DeserializeTransformComponent(entityNode["TransformComponent"]);
                            entityManager.m_registry->emplace_or_replace<TransformComponent>(entity, component);
                        }

                        if (entityNode["MaterialComponent"]) {
                            auto component = DeserializeMaterialComponent(entityNode["MaterialComponent"]);
                            entityManager.m_registry->emplace_or_replace<MaterialComponent>(entity, component);
                        }
                    }
                    else {
                        std::cout << "ERROR AL CARGAR EL MODELO con la mesh " << meshModelName << std::endl;
                        continue;
                    }
                }
                else {
                    entity = entityManager.m_registry->create(); // Crear una entidad vacía
                }

                // Almacenar la correspondencia entre el ID de la entidad y la entidad creada
                entityMap[entityID] = entity;

                // Deserializar componentes adicionales
                if (entityNode["IDComponent"]) {
                    auto component = DeserializeIDComponent(entityNode["IDComponent"]);
                    entityManager.m_registry->emplace_or_replace<IDComponent>(entity, component);
                }

                if (entityNode["TagComponent"]) {
                    auto component = DeserializeTagComponent(entityNode["TagComponent"]);
                    entityManager.m_registry->emplace_or_replace<TagComponent>(entity, component);
                }

                if (entityNode["ScriptComponent"]) {
                    auto component = DeserializeScriptComponent(entityNode["ScriptComponent"]);
                    entityManager.m_registry->emplace_or_replace<ScriptComponent>(entity, component);
                }
            }

            // Segundo pase: Configurar las relaciones de jerarquía (padres e hijos)
            for (auto entityNode : entities) {
                uint32_t entityID = entityNode["Entity"].as<uint32_t>();
                entt::entity entity = entityMap[entityID];

                // Deserializar y asignar ParentComponent
                if (entityNode["ParentComponent"]) {
                    uint32_t parentID = entityNode["ParentComponent"]["Parent"].as<uint32_t>();
                    if (entityMap.find(parentID) != entityMap.end()) {
                        entt::entity parentEntity = entityMap[parentID];
                        entityManager.m_registry->emplace_or_replace<ParentComponent>(entity, ParentComponent{ parentEntity });
                        entityManager.m_registry->emplace_or_replace<ChildComponent>(parentEntity).children.push_back(entity);
                    }
                }

                // Deserializar y asignar ChildComponent
                if (entityNode["ChildComponent"]) {
                    auto childrenNode = entityNode["ChildComponent"];
                    for (auto childIDNode : childrenNode) {
                        uint32_t childID = childIDNode.as<uint32_t>();
                        if (entityMap.find(childID) != entityMap.end()) {
                            entt::entity childEntity = entityMap[childID];
                            entityManager.m_registry->emplace_or_replace<ChildComponent>(entity).children.push_back(childEntity);
                            entityManager.m_registry->emplace_or_replace<ParentComponent>(childEntity, ParentComponent{ entity });
                        }
                    }
                }
            }
        }
    }



    //void Scene::DeserializeScene() {
    //    YAML::Node data = YAML::LoadFile("assets/Scenes/" + sceneName + ".yaml");
    //    if (!data["Scene"]) {
    //        return;
    //    }

    //    // Deserializar los modelos cargados en memoria
    //    if (data["Models"]) {
    //        DeserializeAllModels(data["Models"]);
    //    }

    //    auto& entityManager = EntityManager::GetInstance();
    //    auto entities = data["Entities"];

    //    if (entities) {
    //        for (auto entityNode : entities) {

    //            uint32_t entityID = entityNode["Entity"].as<uint32_t>();

    //            entt::entity entity;



    //            if (entityNode["MeshComponent"]) 
    //            {
    //                std::string meshModelName = entityNode["MeshComponent"]["MeshName"].as<std::string>();
    //                Ref<Model> model = AssetsManager::GetInstance().GetModelByMeshName(meshModelName);
    //                if (model != nullptr)
    //                {
    //                    entity = entityManager.CreateGameObjectFromModel(model, entt::null);

    //                    // Deserializa y asigna componentes
    //                    if (entityNode["TransformComponent"]) 
    //                    {
    //                        auto component = DeserializeTransformComponent(entityNode["TransformComponent"]);
    //                        entityManager.m_registry->emplace_or_replace<TransformComponent>(entity, component);
    //                    }

    //                    if (entityNode["MaterialComponent"]) {
    //                        auto component = DeserializeMaterialComponent(entityNode["MaterialComponent"]);
    //                        entityManager.m_registry->emplace_or_replace<MaterialComponent>(entity, component);
    //                    }
    //                }
    //                else
    //                {
    //                    std::cout << "ERROR AL CARGAR EL MODELO con la mesh " << meshModelName  << std::endl;
    //                }
    //                
    //            }
    //            else
    //            {

    //            }

    //            
    //        }
    //    }
    //}
}



//if (entityNode["TagComponent"]) {
                //    auto component = DeserializeTagComponent(entityNode["TagComponent"]);
                //    entity = entityManager.CreateEmptyGameObject(component.Tag);
                //    entityManager.m_registry->emplace_or_replace<TagComponent>(entity, component);
                //}
                ////else
                ////{
                ////    entity = entityManager.CreateEmptyGameObject();
                ////    entityManager.AddComponent<TagComponent>(entity);
                ////    
                ////}

                //// Deserializa el IDComponent
                //UUID uuid = entityNode["IDComponent"]["ID"].as<UUID>();
                //entityManager.m_registry->emplace<IDComponent>(entity, uuid);

                //// Verifica que la entidad es válida antes de continuar
                //if (!entityManager.m_registry->valid(entity)) {
                //    std::cerr << "Error: entidad inválida al deserializar." << std::endl;
                //    continue;
                //}

                //// Deserializa y asigna componentes
                //if (entityNode["TransformComponent"]) {
                //    auto component = DeserializeTransformComponent(entityNode["TransformComponent"]);
                //    entityManager.m_registry->emplace_or_replace<TransformComponent>(entity, component);
                //}

                //if (entityNode["MaterialComponent"]) {
                //    auto component = DeserializeMaterialComponent(entityNode["MaterialComponent"]);
                //    entityManager.m_registry->emplace_or_replace<MaterialComponent>(entity, component);
                //}

                //if (entityNode["MeshComponent"]) {
                //    auto component = DeserializeMeshComponent(entityNode["MeshComponent"]);
                //    entityManager.m_registry->emplace_or_replace<MeshComponent>(entity, component);
                //}

                //if (entityNode["AABBComponent"]) {
                //    auto component = DeserializeAABBComponent(entityNode["AABBComponent"]);
                //    entityManager.m_registry->emplace_or_replace<AABBComponent>(entity, component);
                //}

                //

                //if (entityNode["ScriptComponent"]) {
                //    auto component = DeserializeScriptComponent(entityNode["ScriptComponent"]);
                //    entityManager.m_registry->emplace_or_replace<ScriptComponent>(entity, component);
                //}