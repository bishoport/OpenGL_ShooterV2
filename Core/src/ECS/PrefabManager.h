#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <yaml-cpp/yaml.h>
//#include "EntityManager.h"
#include "ECS.h"

namespace libCore {

    class PrefabManager {
    public:

        static PrefabManager& GetInstance() {
            static PrefabManager instance;
            return instance;
        }

        void CreatePrefab(const std::string& name, entt::entity entity);
        entt::entity InstantiatePrefab(const std::string& name);

        // Serialización
        void SerializePrefab(const std::string& name, const std::string& filePath);
        void DeserializePrefab(const std::string& filePath);

    private:
        PrefabManager() {}
        std::unordered_map<std::string, YAML::Node> m_Prefabs;
    };

}


