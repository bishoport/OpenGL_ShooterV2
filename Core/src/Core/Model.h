#pragma once

#include "../LibCoreHeaders.h"

#include "Mesh.h"
#include "Material.h"
#include "skeletal/AnimData.h"
#include "../ECS/ECS.h"

namespace libCore
{
    class Model
    {
    public:

        libCore::Transform transform;

        //--MODEL DATA
        std::vector<Ref<Mesh>> meshes;
        std::vector<Ref<Material>> materials;

        //--SKELETAL DATA
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;

        auto& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCount() { return m_BoneCounter; }

        //--METHODS
        void Draw(const std::string& shader);
    };
}
