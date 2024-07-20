#include "PrefabManager.h"
#include "EntityManager.hpp"

namespace YAML {

    template<>
    struct convert<glm::vec3> {
        static Node encode(const glm::vec3& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs) {
            if (!node.IsSequence() || node.size() != 3) {
                return false;
            }
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}



namespace libCore {

    void PrefabManager::CreatePrefab(const std::string& name, entt::entity entity) {

        YAML::Node prefabNode;
        auto& registry = EntityManager::GetInstance().m_registry;

        if (registry->has<TransformComponent>(entity)) {
            auto& transformComponent = registry->get<TransformComponent>(entity);
            YAML::Node transformNode;
            transformNode["position"] = transformComponent.transform->position;
            transformNode["rotation"] = transformComponent.transform->eulerAngles;
            transformNode["scale"] = transformComponent.transform->scale;
            prefabNode["TransformComponent"] = transformNode;
        }

        if (registry->has<MeshComponent>(entity)) {
            auto& meshComponent = registry->get<MeshComponent>(entity);
            prefabNode["MeshComponent"] = meshComponent.mesh->meshName;  // Assuming Mesh has a GetName() method
        }

        if (registry->has<MaterialComponent>(entity)) {
            auto& materialComponent = registry->get<MaterialComponent>(entity);
            YAML::Node materialNode;
            materialNode["materialName"] = materialComponent.material->materialName;
            materialNode["shaderName"] = materialComponent.material->shaderName;
            materialNode["albedoColor"] = materialComponent.material->albedoColor;
            materialNode["normalStrength"] = materialComponent.material->normalStrength;
            materialNode["metallicValue"] = materialComponent.material->metallicValue;
            materialNode["roughnessValue"] = materialComponent.material->roughnessValue;
            materialNode["aoValue"] = materialComponent.material->aoValue;
            prefabNode["MaterialComponent"] = materialNode;
        }

        // Uncomment and complete if ScriptComponent is used
        // if (registry->has<ScriptComponent>(entity)) {
        //     auto& scriptComponent = registry->get<ScriptComponent>(entity);
        //     prefabNode["ScriptComponent"] = scriptComponent.instance->GetName(); // Assuming Script has a GetName() method
        // }

        m_Prefabs[name] = prefabNode;
    }

    entt::entity PrefabManager::InstantiatePrefab(const std::string& name) {
        const auto& prefabNode = m_Prefabs.at(name);  // Use at() for better error handling
        auto entity = EntityManager::GetInstance().CreateEmptyGameObject();

        auto& registry = EntityManager::GetInstance().m_registry;

        if (prefabNode["TransformComponent"]) {
            const auto& transformNode = prefabNode["TransformComponent"];
            TransformComponent transformComponent;
            transformComponent.transform = std::make_shared<Transform>();
            transformComponent.transform->position = transformNode["position"].as<glm::vec3>();
            transformComponent.transform->eulerAngles = transformNode["rotation"].as<glm::vec3>();
            transformComponent.transform->scale = transformNode["scale"].as<glm::vec3>();
            registry->emplace<TransformComponent>(entity, transformComponent);
        }

        //if (prefabNode["MeshComponent"]) {
        //    std::string meshName = prefabNode["MeshComponent"].as<std::string>();
        //    MeshComponent meshComponent;
        //    meshComponent.mesh = AssetsManager::GetInstance().GetMesh(meshName);
        //    registry->emplace<MeshComponent>(entity, meshComponent);
        //}

        if (prefabNode["MaterialComponent"]) {
            const auto& materialNode = prefabNode["MaterialComponent"];
            MaterialComponent materialComponent;
            materialComponent.material = std::make_shared<Material>(materialNode["materialName"].as<std::string>());
            materialComponent.material->shaderName = materialNode["shaderName"].as<std::string>();
            materialComponent.material->albedoColor = materialNode["albedoColor"].as<glm::vec3>();
            materialComponent.material->normalStrength = materialNode["normalStrength"].as<float>();
            materialComponent.material->metallicValue = materialNode["metallicValue"].as<float>();
            materialComponent.material->roughnessValue = materialNode["roughnessValue"].as<float>();
            materialComponent.material->aoValue = materialNode["aoValue"].as<float>();
            registry->emplace<MaterialComponent>(entity, materialComponent);
        }

        //if (prefabNode["ScriptComponent"]) {
        //    std::string scriptName = prefabNode["ScriptComponent"].as<std::string>();
        //    ScriptComponent scriptComponent;
        //    scriptComponent.instance = ScriptLibrary::GetInstance().CreateScript(scriptName);
        //    registry->emplace<ScriptComponent>(entity, scriptComponent);
        //}

        return entity;
    }

    void PrefabManager::SerializePrefab(const std::string& name, const std::string& filePath) {
        std::ofstream fout(filePath);
        fout << m_Prefabs[name];
        fout.close();
    }

    void PrefabManager::DeserializePrefab(const std::string& filePath) {
        std::ifstream fin(filePath);
        YAML::Node prefabNode = YAML::Load(fin);
        std::string name = prefabNode["name"].as<std::string>();
        m_Prefabs[name] = prefabNode;
    }

} // namespace libCore
