#pragma once

#include "../LibCoreHeaders.h"
#include <string>
#include "Texture.h"

namespace libCore
{
    class Material
    {
    public:
        std::string materialName = "none";
        std::string shaderName = "default";

        //VALUES
        glm::vec3 albedoColor = glm::vec3(1.0f, 1.0f, 1.0f);
        float normalStrength = 2.0f;
        float metallicValue = 0.0f;
        float roughnessValue = 0.5f;
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