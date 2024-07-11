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

        bool showAABB = false;

        libCore::Transform transform;
        std::string name = "no_name_model";

        //--MODEL DATA
        Ref<Model> modelParent = nullptr;
        std::vector<Ref<Model>> childs;
        std::vector<Ref<Mesh>> meshes;
        std::vector<Ref<Material>> materials;

        //--METHODS
        void Draw(const std::string& shader);
        void DrawAABB(const std::string& shader);
        void UpdateAABB();



        //--SKELETAL DATA
        bool skeletal = false;
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;

        auto& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCount() { return m_BoneCounter; }
    };
}
