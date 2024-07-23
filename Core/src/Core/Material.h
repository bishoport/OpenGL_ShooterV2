#pragma once

#include "../LibCoreHeaders.h"
#include <string>
#include "Texture.h"
#include "../tools/AssetsManager.h"

namespace libCore
{
    class Material
    {
    public:
        // Constructor predeterminado
        Material()
            : materialName("default_material"), shaderName("default"),
              albedoColor(1.0f, 1.0f, 1.0f), normalStrength(1.0f),
              metallicValue(0.0f), roughnessValue(0.7f), aoValue(1.0f)
        {
            albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
            normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
            metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
            roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
        }

        Material(const std::string& matName)
            : materialName(matName), shaderName("default"),
              albedoColor(1.0f, 1.0f, 1.0f), normalStrength(1.0f),
              metallicValue(0.0f), roughnessValue(0.8f), aoValue(1.0f)
        {
            albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
            normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
            metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
            roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
        }

        std::string materialName;
        std::string shaderName;

        // Valores
        glm::vec3 albedoColor;
        float normalStrength;
        float metallicValue;
        float roughnessValue;
        float aoValue;

        // Texturas
        Ref<Texture> albedoMap;
        Ref<Texture> normalMap;
        Ref<Texture> metallicMap;
        Ref<Texture> roughnessMap;
        Ref<Texture> aOMap;
    };
}
