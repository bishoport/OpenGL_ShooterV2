#include "Scene.h"
#include "EntityManager.h"
#include <fstream>
#include "../Core/Transform.h"
#include "../Core/Material.h"
#include "SerializatorManager.h"
#include "../Scripting/LuaManager.h"

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




    //--SAVE/LOAD PROJECT
    void Scene::SerializeScene(std::string _sceneName, bool saveResource) {
        sceneName = _sceneName;

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";

        if (saveResource)
        {
            // Serializar los modelos cargados en memoria
            auto models = SerializeAllModels();
            if (models.size() > 0) {
                out << YAML::Key << "Models" << YAML::Value << models;
            }
            else {
                out << YAML::Key << "Models" << YAML::Value << YAML::BeginSeq << YAML::EndSeq; // Lista vacía
            }
        }
        else {
            out << YAML::Key << "Models" << YAML::Value << YAML::BeginSeq << YAML::EndSeq; // Lista vacía
        }


        // Serializar las entidades
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        auto& entityManager = EntityManager::GetInstance();
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

            // Serializar TransformComponent
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

            // Serializar MeshComponent
            if (entityManager.HasComponent<MeshComponent>(entity)) {
                out << YAML::Key << "MeshComponent";
                out << YAML::Value << SerializeMeshComponent(entityManager.GetComponent<MeshComponent>(entity));
            }

            // Serializar MaterialComponent
            if (entityManager.HasComponent<MaterialComponent>(entity)) {
                out << YAML::Key << "MaterialComponent";
                out << YAML::Value << SerializeMaterialComponent(entityManager.GetComponent<MaterialComponent>(entity));
            }

            // Serializar ScriptComponent con múltiples scripts
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

    void Scene::DeserializeScene(std::string _sceneName, bool loadResource) {
        YAML::Node data = YAML::LoadFile("assets/Scenes/" + _sceneName + ".yaml");
        if (!data["Scene"]) {
            return;
        }

        if (loadResource)
        {
            // Deserializar los modelos cargados en memoria
            if (data["Models"]) {
                DeserializeAllModels(data["Models"]);
            }
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

                if (entityNode["TransformComponent"]) {
                    auto transform_component = DeserializeTransformComponent(entityNode["TransformComponent"]);
                    entityManager.m_registry->emplace_or_replace<TransformComponent>(entity, transform_component);
                }

                if (entityNode["MeshComponent"]) {
                    std::string meshModelName = entityNode["MeshComponent"]["MeshName"].as<std::string>();
                    Ref<Model> model = AssetsManager::GetInstance().GetModelByMeshName(meshModelName);

                    if (model != nullptr) {
                        // Asignar los componentes de MeshComponent si el modelo tiene mallas
                        if (!model->meshes.empty()) {
                            for (auto& mesh : model->meshes) {
                                auto& meshComponent = entityManager.m_registry->emplace<MeshComponent>(entity, mesh, model, true);
                                glm::mat4 instanceMatrix = glm::translate(glm::mat4(1.0f), model->transform->position);
                                meshComponent.instanceMatrices.push_back(instanceMatrix);
                                auto& abbComponent = entityManager.m_registry->emplace<AABBComponent>(entity);
                                abbComponent.aabb->CalculateAABB(mesh->vertices);
                            }
                        }
                    }
                    else {
                        std::cout << "ERROR AL CARGAR EL MODELO con la mesh " << meshModelName << std::endl;
                        continue;
                    }
                }

                if (entityNode["MaterialComponent"]) {
                    auto component = DeserializeMaterialComponent(entityNode["MaterialComponent"]);
                    Ref<Material> mat = AssetsManager::GetInstance().getMaterial(component.material->materialName);
                    if (mat != nullptr) {
                        component.material = mat;
                    }
                    entityManager.m_registry->emplace_or_replace<MaterialComponent>(entity, component);
                }

                // Deserializar ScriptComponent con múltiples scripts
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
    //---------------------------------------------------------------------------------------------

}
