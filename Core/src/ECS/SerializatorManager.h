#pragma once
#include "../LibCoreHeaders.h"
#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/emitterstyle.h"
#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"

#include "../tools/AssetsManager.h"
#include "../Core/Transform.h"
#include "../Core/Material.h"
#include "../Core/Texture.h"
#include "ECS.h"


namespace YAML {

    //--Vector3
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

    //--TRANSFORM
    template<>
    struct convert<libCore::Transform> {
        static Node encode(const libCore::Transform& rhs) {
            Node node;
            node["Position"] = rhs.position;
            node["Rotation"] = rhs.eulerAngles;
            node["Scale"] = rhs.scale;
            return node;
        }

        static bool decode(const Node& node, libCore::Transform& rhs) {
            if (!node["Position"] || !node["Rotation"] || !node["Scale"]) {
                return false;
            }
            rhs.position = node["Position"].as<glm::vec3>();
            rhs.eulerAngles = node["Rotation"].as<glm::vec3>();
            rhs.scale = node["Scale"].as<glm::vec3>();
            return true;
        }
    };

    //--MATERIAL
    template<>
    struct convert<libCore::Material> {
        static Node encode(const libCore::Material& rhs) {
            Node node;
            node["MaterialName"] = rhs.materialName;
            node["ShaderName"] = rhs.shaderName;
            node["AlbedoColor"] = rhs.albedoColor;
            node["NormalStrength"] = rhs.normalStrength;
            node["MetallicValue"] = rhs.metallicValue;
            node["RoughnessValue"] = rhs.roughnessValue;
            node["AoValue"] = rhs.aoValue;
            return node;
        }

        static bool decode(const Node& node, libCore::Material& rhs) {
            if (!node["MaterialName"] || !node["ShaderName"] || !node["AlbedoColor"] || !node["NormalStrength"] ||
                !node["MetallicValue"] || !node["RoughnessValue"] || !node["AoValue"]) {
                return false;
            }
            rhs.materialName = node["MaterialName"].as<std::string>();
            rhs.shaderName = node["ShaderName"].as<std::string>();
            rhs.albedoColor = node["AlbedoColor"].as<glm::vec3>();
            rhs.normalStrength = node["NormalStrength"].as<float>();
            rhs.metallicValue = node["MetallicValue"].as<float>();
            rhs.roughnessValue = node["RoughnessValue"].as<float>();
            rhs.aoValue = node["AoValue"].as<float>();
            return true;
        }
    };

    //--TEXTURE
    template<>
    struct convert<libCore::Texture> {
        static Node encode(const libCore::Texture& rhs) {
            Node node;
            node["TexturePath"] = rhs.texturePath;
            node["TextureType"] = static_cast<int>(rhs.m_type);
            node["TextureUnit"] = rhs.m_unit;
            return node;
        }

        static bool decode(const Node& node, libCore::Texture& rhs) {
            if (!node["TexturePath"] || !node["TextureType"] || !node["TextureUnit"]) {
                return false;
            }
            rhs.texturePath = node["TexturePath"].as<std::string>();
            rhs.m_type = static_cast<TEXTURE_TYPES>(node["TextureType"].as<int>());
            rhs.m_unit = node["TextureUnit"].as<GLuint>();

            // Aquí debes cargar la textura desde el archivo
            rhs.ID = 0; // Inicializar ID para asegurarse de que la textura se cargue
            // Verifica si la textura ya está cargada en AssetsManager
            auto existingTexture = libCore::AssetsManager::GetInstance().GetTexture(rhs.texturePath);
            if (existingTexture && existingTexture->IsValid()) {
                rhs = *existingTexture;
            }
            else {
                // Si no está cargada, crea una nueva textura
                rhs = libCore::Texture(rhs.texturePath.c_str(), rhs.m_type, rhs.m_unit);
            }

            return true;
        }
    };
}

namespace libCore {

    YAML::Node SerializeTransformComponent(const TransformComponent& transformComponent) {
        YAML::Node node;
        node["Transform"] = *transformComponent.transform;
        return node;
    }

    TransformComponent DeserializeTransformComponent(const YAML::Node& node) {
        TransformComponent transformComponent;
        transformComponent.transform = CreateRef<Transform>(node["Transform"].as<Transform>());
        return transformComponent;
    }

    YAML::Node SerializeMaterialComponent(const MaterialComponent& materialComponent) {
        YAML::Node node;
        node["Material"] = *materialComponent.material;

        // Serializar texturas
        if (materialComponent.material->albedoMap) {
            node["AlbedoMap"] = *materialComponent.material->albedoMap;
        }
        if (materialComponent.material->normalMap) {
            node["NormalMap"] = *materialComponent.material->normalMap;
        }
        if (materialComponent.material->metallicMap) {
            node["MetallicMap"] = *materialComponent.material->metallicMap;
        }
        if (materialComponent.material->roughnessMap) {
            node["RoughnessMap"] = *materialComponent.material->roughnessMap;
        }
        if (materialComponent.material->aOMap) {
            node["AOMap"] = *materialComponent.material->aOMap;
        }

        return node;
    }

    MaterialComponent DeserializeMaterialComponent(const YAML::Node& node) {
        MaterialComponent materialComponent;
        materialComponent.material = CreateRef<Material>();
        *materialComponent.material = node["Material"].as<Material>();

        // Deserializar texturas
        if (node["AlbedoMap"]) {
            materialComponent.material->albedoMap = CreateRef<Texture>();
            *materialComponent.material->albedoMap = node["AlbedoMap"].as<Texture>();
        }
        if (node["NormalMap"]) {
            materialComponent.material->normalMap = CreateRef<Texture>();
            *materialComponent.material->normalMap = node["NormalMap"].as<Texture>();
        }
        if (node["MetallicMap"]) {
            materialComponent.material->metallicMap = CreateRef<Texture>();
            *materialComponent.material->metallicMap = node["MetallicMap"].as<Texture>();
        }
        if (node["RoughnessMap"]) {
            materialComponent.material->roughnessMap = CreateRef<Texture>();
            *materialComponent.material->roughnessMap = node["RoughnessMap"].as<Texture>();
        }
        if (node["AOMap"]) {
            materialComponent.material->aOMap = CreateRef<Texture>();
            *materialComponent.material->aOMap = node["AOMap"].as<Texture>();
        }

        return materialComponent;
    }

}
