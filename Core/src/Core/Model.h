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

        // Constructor por defecto
        Model() = default;

        Model(ImportModelData _importModelData) : importModelData(_importModelData){}

        ImportModelData importModelData;
        Ref<Transform> transform = CreateRef<Transform>();
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
    };
}