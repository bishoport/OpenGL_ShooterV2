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

        Material()
        {

        }

        Material(const std::string& matName)
        {
            materialName = matName;

            //--ALBEDO
            albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
            //-------------------

            //--NORMAL
            normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
            //-------------------


            //--METALLIC
            metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
            //-------------------


            //--ROUGHNESS
            roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
            //-------------------
        }

        std::string materialName = "none";
        std::string shaderName = "default";

        //VALUES
        glm::vec3 albedoColor = glm::vec3(1.0f, 1.0f, 1.0f);
        float normalStrength = 1.0f;
        float metallicValue = 0.0f;
        float roughnessValue = 2.0f;
        float aoValue = 1.0f;

        //TEXTURES
        Ref<Texture> albedoMap;
        Ref<Texture> normalMap;
        Ref<Texture> metallicMap;
        Ref<Texture> roughnessMap;
        Ref<Texture> aOMap;
        //-------------------------------------------------------
    };
}