#include "Scene.h"
#include "EntityManager.h"
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


    void Scene::SerializeScene(std::string _sceneName) {

        sceneName = _sceneName;

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";

        // Serializar los modelos cargados en memoria
        out << YAML::Key << "Models" << YAML::Value << SerializeAllModels();

        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        auto& entityManager = EntityManager::GetInstance();

        entityManager.DebugPrintAllEntityHierarchies();

        entityManager.m_registry->each([&](auto entity) {

            out << YAML::BeginMap; // Entity
            out << YAML::Key << "Entity" << YAML::Value << (uint32_t)entity; // Serialize entity ID

            if (entityManager.HasComponent<IDComponent>(entity)) {
                out << YAML::Key << "IDComponent";
                out << YAML::Value << SerializeIDComponent(entityManager.GetComponent<IDComponent>(entity));
            }

            if (entityManager.HasComponent<TagComponent>(entity)) {
                out << YAML::Key << "TagComponent";
                out << YAML::Value << SerializeTagComponent(entityManager.GetComponent<TagComponent>(entity));
            }

            // Serialize components
            if (entityManager.HasComponent<TransformComponent>(entity)) {
                out << YAML::Key << "TransformComponent";
                out << YAML::Value << SerializeTransformComponent(entityManager.GetComponent<TransformComponent>(entity));
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

            if (entityManager.HasComponent<MeshComponent>(entity)) {
                out << YAML::Key << "MeshComponent";
                out << YAML::Value << SerializeMeshComponent(entityManager.GetComponent<MeshComponent>(entity));
            }

            if (entityManager.HasComponent<MaterialComponent>(entity)) {
                out << YAML::Key << "MaterialComponent";
                out << YAML::Value << SerializeMaterialComponent(entityManager.GetComponent<MaterialComponent>(entity));
            }

            if (entityManager.HasComponent<ScriptComponent>(entity)) {
                out << YAML::Key << "ScriptComponent";
                out << YAML::Value << SerializeScriptComponent(entityManager.GetComponent<ScriptComponent>(entity));
            }
            out << YAML::EndMap; // Entity
        });

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout("assets/Scenes/" + sceneName + ".yaml");
        fout << out.c_str();
    }



    void Scene::DeserializeScene(std::string _sceneName) {

        YAML::Node data = YAML::LoadFile("assets/Scenes/" + _sceneName + ".yaml");
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

                entt::entity entity = entityManager.CreateEmptyGameObject();


                if (entityNode["TagComponent"]) {
                    auto tag_component = DeserializeTagComponent(entityNode["TagComponent"]);
                    entityManager.m_registry->emplace_or_replace<TagComponent>(entity, tag_component);
                }

                if (entityNode["IDComponent"]) {
                    auto id_component = DeserializeIDComponent(entityNode["IDComponent"]);
                    entityManager.m_registry->emplace_or_replace<IDComponent>(entity, id_component);
                }

                //std::cout << "Procesando Entity YALM_ID: " << entityID << std::endl;
                //std::cout << "Procesando Entity TAG: " << entityManager.GetComponent<TagComponent>(entity).Tag.c_str() << std::endl;
                //std::cout << "Procesando Entity UUID: " << entityManager.GetComponent<IDComponent>(entity).ID.ToString() << std::endl;


                if (entityNode["TransformComponent"]) {
                    auto transform_component = DeserializeTransformComponent(entityNode["TransformComponent"]);
                    entityManager.m_registry->emplace_or_replace<TransformComponent>(entity, transform_component);
                }

                if (entityNode["MeshComponent"]) 
                {
                    std::string meshModelName = entityNode["MeshComponent"]["MeshName"].as<std::string>();
                    
                    Ref<Model> model = AssetsManager::GetInstance().GetModelByMeshName(meshModelName);
                    
                    if (model != nullptr) {
                        // Asignar los componentes de MeshComponent si el modelo tiene mallas
                        if (!model->meshes.empty()) 
                        {
                            for (auto& mesh : model->meshes) {
                                auto& meshComponent = entityManager.m_registry->emplace<MeshComponent>(entity, mesh, model, true);
                                // Agregar matriz de instancia
                                glm::mat4 instanceMatrix = glm::translate(glm::mat4(1.0f), model->transform->position);
                                meshComponent.instanceMatrices.push_back(instanceMatrix);
                                auto& abbComponent = entityManager.m_registry->emplace<AABBComponent>(entity);
                                abbComponent.aabb->CalculateAABB(mesh->vertices);
                            }
                        }
                    }
                    else 
                    {
                        std::cout << "ERROR AL CARGAR EL MODELO con la mesh " << meshModelName << std::endl;
                        continue;
                    }
                }

                if (entityNode["MaterialComponent"])
                {
                    auto component = DeserializeMaterialComponent(entityNode["MaterialComponent"]);
                    Ref<Material> mat = AssetsManager::GetInstance().getMaterial(component.material->materialName);
                    if (mat != nullptr)
                    {
                        component.material = mat;
                    }
                    entityManager.m_registry->emplace_or_replace<MaterialComponent>(entity, component);
                }

                if (entityNode["ScriptComponent"]) {
                    auto component = DeserializeScriptComponent(entityNode["ScriptComponent"]);
                    entityManager.m_registry->emplace_or_replace<ScriptComponent>(entity, component);
                }

                // Almacenar la correspondencia entre el ID de la entidad y la entidad creada
                entityMap[entityID] = entity;
            }



            // Segundo pase: Configurar las relaciones de jerarquía (padres e hijos)
            for (auto entityNode : entities) {
                uint32_t entityID = entityNode["Entity"].as<uint32_t>();
                entt::entity entity = entityMap[entityID];

                // Deserializar y asignar ParentComponent usando AddChild
                if (entityNode["ParentComponent"]) {
                    uint32_t parentID = entityNode["ParentComponent"]["Parent"].as<uint32_t>();
                    if (entityMap.find(parentID) != entityMap.end()) {
                        entt::entity parentEntity = entityMap[parentID];
                        entityManager.AddChild(parentEntity, entity); // Usar la función AddChild del EntityManager
                    }
                }

                // Deserializar y asignar ChildComponent usando AddChild
                if (entityNode["ChildComponent"]) {
                    auto childrenNode = entityNode["ChildComponent"];
                    for (auto childIDNode : childrenNode) {
                        uint32_t childID = childIDNode.as<uint32_t>();
                        if (entityMap.find(childID) != entityMap.end()) {
                            entt::entity childEntity = entityMap[childID];
                            entityManager.AddChild(entity, childEntity); // Usar la función AddChild del EntityManager
                        }
                    }
                }
            }
        }
    }




    //void SerializeEntity(YAML::Emitter& out, entt::entity entity) {
    //    auto& entityManager = EntityManager::GetInstance();

    //    out << YAML::BeginMap; // Entity
    //    out << YAML::Key << "Entity" << YAML::Value << entityManager.GetComponent<IDComponent>(entity).ID; // Serialize entity UUID

    //    // Serialize components
    //    if (entityManager.HasComponent<TransformComponent>(entity)) {
    //        out << YAML::Key << "TransformComponent";
    //        out << YAML::Value << SerializeTransformComponent(entityManager.GetComponent<TransformComponent>(entity));
    //    }

    //    if (entityManager.HasComponent<MaterialComponent>(entity)) {
    //        out << YAML::Key << "MaterialComponent";
    //        out << YAML::Value << SerializeMaterialComponent(entityManager.GetComponent<MaterialComponent>(entity));
    //    }

    //    if (entityManager.HasComponent<MeshComponent>(entity)) {
    //        out << YAML::Key << "MeshComponent";
    //        out << YAML::Value << SerializeMeshComponent(entityManager.GetComponent<MeshComponent>(entity));
    //    }

    //    if (entityManager.HasComponent<AABBComponent>(entity)) {
    //        out << YAML::Key << "AABBComponent";
    //        out << YAML::Value << SerializeAABBComponent(entityManager.GetComponent<AABBComponent>(entity));
    //    }

    //    if (entityManager.HasComponent<TagComponent>(entity)) {
    //        out << YAML::Key << "TagComponent";
    //        out << YAML::Value << SerializeTagComponent(entityManager.GetComponent<TagComponent>(entity));
    //    }

    //    if (entityManager.HasComponent<ScriptComponent>(entity)) {
    //        out << YAML::Key << "ScriptComponent";
    //        out << YAML::Value << SerializeScriptComponent(entityManager.GetComponent<ScriptComponent>(entity));
    //    }

    //    // Serializar el ParentComponent
    //    if (entityManager.HasComponent<ParentComponent>(entity)) {
    //        out << YAML::Key << "ParentComponent";
    //        auto& parentComponent = entityManager.GetComponent<ParentComponent>(entity);
    //        out << YAML::BeginMap;
    //        out << YAML::Key << "Parent" << YAML::Value << entityManager.GetComponent<IDComponent>(parentComponent.parent).ID; // Serialize parent UUID
    //        out << YAML::EndMap;
    //    }

    //    // Serializar el ChildComponent
    //    if (entityManager.HasComponent<ChildComponent>(entity)) {
    //        out << YAML::Key << "ChildComponent";
    //        auto& childComponent = entityManager.GetComponent<ChildComponent>(entity);
    //        out << YAML::BeginSeq;
    //        for (auto child : childComponent.children) {
    //            out << entityManager.GetComponent<IDComponent>(child).ID; // Serialize child UUIDs
    //        }
    //        out << YAML::EndSeq;
    //    }

    //    out << YAML::EndMap; // Entity
    //}
    //void Scene::SerializeScene(std::string _sceneName) {
    //    sceneName = _sceneName;

    //    YAML::Emitter out;
    //    out << YAML::BeginMap;
    //    out << YAML::Key << "Scene" << YAML::Value << "Untitled";

    //    // Serializar los modelos cargados en memoria
    //    out << YAML::Key << "Models" << YAML::Value << SerializeAllModels();

    //    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

    //    auto& entityManager = EntityManager::GetInstance();

    //    // Serializar todas las entidades
    //    entityManager.m_registry->view<IDComponent>().each([&](entt::entity entity, auto& idComponent) {
    //        SerializeEntity(out, entity);  // Asegúrate de que se está pasando un entt::entity
    //        });

    //    out << YAML::EndSeq;
    //    out << YAML::EndMap;

    //    std::ofstream fout("assets/Scenes/" + sceneName + ".yaml");
    //    fout << out.c_str();
    //}
    // 
    // 
    // 
    // 
    // 
    // 
    //void Scene::DeserializeScene(std::string _sceneName) {
    //    YAML::Node data = YAML::LoadFile("assets/Scenes/" + _sceneName + ".yaml");
    //    if (!data["Scene"]) {
    //        return;
    //    }

    //    // Deserializar los modelos cargados en memoria
    //    if (data["Models"]) {
    //        DeserializeAllModels(data["Models"]);
    //    }

    //    auto& entityManager = EntityManager::GetInstance();
    //    auto entities = data["Entities"];

    //    // Mapa para mantener la correspondencia entre los UUIDs de las entidades y las entidades deserializadas
    //    std::unordered_map<UUID, entt::entity> entityMap;

    //    // Primer pase: Crear todas las entidades y deserializar los componentes básicos
    //    for (auto entityNode : entities) {
    //        UUID entityID = entityNode["Entity"].as<UUID>();

    //        entt::entity entity;

    //        // Si tiene un MeshComponent, crear la entidad como un modelo
    //        if (entityNode["MeshComponent"]) {
    //            std::string meshModelName = entityNode["MeshComponent"]["MeshName"].as<std::string>();
    //            Ref<Model> model = AssetsManager::GetInstance().GetModelByMeshName(meshModelName);
    //            if (model != nullptr) {
    //                entity = entityManager.CreateGameObjectFromModel(model, entt::null);

    //                // Deserializa y asigna componentes
    //                if (entityNode["TransformComponent"]) {
    //                    auto component = DeserializeTransformComponent(entityNode["TransformComponent"]);
    //                    entityManager.m_registry->emplace_or_replace<TransformComponent>(entity, component);
    //                }

    //                if (entityNode["MaterialComponent"]) {
    //                    auto component = DeserializeMaterialComponent(entityNode["MaterialComponent"]);
    //                    Ref<Material> mat = AssetsManager::GetInstance().getMaterial(component.material->materialName);
    //                    if (mat != nullptr)
    //                    {
    //                        component.material = mat;
    //                    }
    //                    entityManager.m_registry->emplace_or_replace<MaterialComponent>(entity, component);
    //                }
    //            }
    //            else {
    //                std::cout << "ERROR AL CARGAR EL MODELO con la mesh " << meshModelName << std::endl;
    //                continue;
    //            }
    //        }
    //        else {
    //            entity = entityManager.m_registry->create(); // Crear una entidad vacía
    //        }

    //        // Almacenar la correspondencia entre el UUID de la entidad y la entidad creada
    //        entityMap[entityID] = entity;

    //        // Deserializar componentes adicionales
    //        if (entityNode["IDComponent"]) {
    //            auto component = DeserializeIDComponent(entityNode["IDComponent"]);
    //            entityManager.m_registry->emplace_or_replace<IDComponent>(entity, component);
    //        }

    //        if (entityNode["TagComponent"]) {
    //            auto component = DeserializeTagComponent(entityNode["TagComponent"]);
    //            entityManager.m_registry->emplace_or_replace<TagComponent>(entity, component);
    //        }

    //        if (entityNode["ScriptComponent"]) {
    //            auto component = DeserializeScriptComponent(entityNode["ScriptComponent"]);
    //            entityManager.m_registry->emplace_or_replace<ScriptComponent>(entity, component);
    //        }
    //    }

    //    // Segundo pase: Configurar las relaciones de jerarquía (padres e hijos)
    //    for (auto entityNode : entities) {
    //        UUID entityID = entityNode["Entity"].as<UUID>();
    //        entt::entity entity = entityMap[entityID];

    //        // Deserializar y asignar ParentComponent
    //        if (entityNode["ParentComponent"]) {
    //            UUID parentID = entityNode["ParentComponent"]["Parent"].as<UUID>();
    //            if (entityMap.find(parentID) != entityMap.end()) {
    //                entt::entity parentEntity = entityMap[parentID];
    //                entityManager.m_registry->emplace_or_replace<ParentComponent>(entity, ParentComponent{ parentEntity });
    //                entityManager.m_registry->emplace_or_replace<ChildComponent>(parentEntity).children.push_back(entity);
    //            }
    //        }

    //        // Deserializar y asignar ChildComponent
    //        if (entityNode["ChildComponent"]) {
    //            auto childrenNode = entityNode["ChildComponent"];
    //            for (auto childIDNode : childrenNode) {
    //                UUID childID = childIDNode.as<UUID>();
    //                if (entityMap.find(childID) != entityMap.end()) {
    //                    entt::entity childEntity = entityMap[childID];
    //                    entityManager.m_registry->emplace_or_replace<ChildComponent>(entity).children.push_back(childEntity);
    //                    entityManager.m_registry->emplace_or_replace<ParentComponent>(childEntity, ParentComponent{ entity });
    //                }
    //            }
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