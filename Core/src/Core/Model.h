#pragma once

#include "../LibCoreHeaders.h"

#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "skeletal/AnimData.h"


namespace libCore
{
    class Model
    {
    public:

        Model()
        {
            transform = CreateRef<Transform>();
        }

        Ref<Transform> transform;
        std::string name = "no_name_model";

        //--MODEL DATA
        Ref<Model> modelParent = nullptr;
        std::vector<Ref<Model>> children;
        std::vector<Ref<Mesh>> meshes;
        std::vector<Ref<Material>> materials;


        //--SKELETAL DATA
        bool skeletal = false;
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;

        auto& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCount() { return m_BoneCounter; }

        // Instancing data
        std::vector<glm::mat4> instanceMatrices;


    };
}