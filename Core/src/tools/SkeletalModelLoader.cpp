#include "SkeletalModelLoader.h"
#include "AssimpGlmHelpers.h"
#include "TextureManager.h"

namespace libCore
{
    Ref<ModelContainer> SkeletalModelLoader::LoadModel(ImportModelData importOptions) {
        auto modelContainer = CreateRef<ModelContainer>();
        modelContainer->skeletal = importOptions.skeletal;

        Assimp::Importer importer;

        std::string completePath = importOptions.filePath + importOptions.fileName;
        std::cout << "Loading Model ->" << completePath << std::endl;

        unsigned int flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
            aiProcess_ValidateDataStructure | aiProcess_GenBoundingBoxes;

        if (importOptions.invertUV) {
            flags |= aiProcess_FlipUVs;
        }

        const aiScene* scene = importer.ReadFile(completePath, flags);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            throw std::runtime_error("Failed to load model");
        }

        aiMatrix4x4 nodeTransform = scene->mRootNode->mTransformation;
        modelContainer->name = importOptions.fileName;

        SkeletalModelLoader::processNode(scene->mRootNode, scene, modelContainer, nodeTransform, importOptions);

        return modelContainer;
    }

    void SkeletalModelLoader::processNode(aiNode* node, const aiScene* scene, Ref<ModelContainer> modelContainer, aiMatrix4x4 _nodeTransform, ImportModelData importOptions) {
        // Mantén una estructura temporal para la malla combinada
        auto combinedModel = CreateRef<Model>();
        auto combinedMesh = CreateRef<Mesh>();

        // Procesa todos los nodos recursivamente, acumulando datos
        processNodeRecursive(node, scene, combinedModel, combinedMesh, _nodeTransform, importOptions);

        // Configura la malla combinada
        combinedModel->meshes.push_back(combinedMesh);
        combinedMesh->SetupMesh();

        // Añade la malla combinada al contenedor del modelo
        modelContainer->models.push_back(combinedModel);
    }

    void SkeletalModelLoader::processNodeRecursive(aiNode* node, const aiScene* scene, Ref<Model> combinedModel, Ref<Mesh> combinedMesh, aiMatrix4x4 _nodeTransform, ImportModelData importOptions) {
        aiMatrix4x4 currentNodeTransform = node->mTransformation * _nodeTransform;
        glm::mat4 glmNodeTransform = aiMatrix4x4ToGlm(currentNodeTransform);

        float globalRotationDeg_X = 0.0f;
        if (importOptions.rotate90) globalRotationDeg_X = -90.0f;

        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(globalRotationDeg_X), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(importOptions.globalScaleFactor));
        glm::mat4 glmFinalTransform = rotationX * scaleMatrix * glmNodeTransform;

        // Procesa todas las mallas del nodo y añade sus datos a `combinedMesh`
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene, combinedModel, combinedMesh, glmFinalTransform, importOptions, i);
        }

        // Procesa los nodos hijos recursivamente
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            processNodeRecursive(node->mChildren[i], scene, combinedModel, combinedMesh, currentNodeTransform, importOptions);
        }
    }

    void SkeletalModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, Ref<Model> combinedModel, Ref<Mesh> combinedMesh, glm::mat4 finalTransform, ImportModelData importOptions, int meshIndex) {
        // Guarda el índice base actual para ajustar las caras
        unsigned int baseIndex = combinedMesh->vertices.size();

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            Vertex vertex;

            if (importOptions.skeletal) {
                SetVertexBoneDataToDefault(vertex);
            }

            // Aplica la transformación al vértice
            glm::vec4 posFixed = finalTransform * glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1);
            vertex.position = glm::vec3(posFixed);

            // Gestiona coordenadas de textura
            vertex.texUV = mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f);

            // Gestiona normales
            glm::vec4 normFixed = finalTransform * glm::vec4(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 1);
            vertex.normal = glm::vec3(normFixed);

            // Gestiona tangentes
            vertex.tangent = mesh->mTangents ? glm::vec3(finalTransform * glm::vec4(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, 1)) : glm::vec3(0.0f);

            // Gestiona bitangentes
            vertex.bitangent = mesh->mBitangents ? glm::vec3(finalTransform * glm::vec4(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z, 1)) : glm::vec3(0.0f);

            combinedMesh->vertices.push_back(vertex);
        }

        // Ajusta los índices de las caras
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                combinedMesh->indices.push_back(baseIndex + face.mIndices[j]);
            }
        }

        // Extrae los datos de huesos si es un modelo esquelético
        ExtractBoneWeightForVertices(combinedModel, combinedMesh, mesh, baseIndex);
    }

    void SkeletalModelLoader::ExtractBoneWeightForVertices(Ref<Model> combinedModel, Ref<Mesh> combinedMesh, aiMesh* mesh, unsigned int baseIndex) {
        auto& boneInfoMap = combinedModel->m_BoneInfoMap;
        int& boneCount = combinedModel->m_BoneCounter;

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

            // Si no existe, añade la información del nuevo hueso
            if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            }
            else {
                boneID = boneInfoMap[boneName].id;
            }
            assert(boneID != -1);

            // Ajusta el índice de los vértices utilizando `baseIndex`
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
                int originalVertexId = weights[weightIndex].mVertexId;
                int combinedVertexId = baseIndex + originalVertexId;
                float weight = weights[weightIndex].mWeight;

                assert(combinedVertexId < combinedMesh->vertices.size());
                SetVertexBoneData(combinedMesh->vertices[combinedVertexId], boneID, weight);
            }
        }
    }



    void SkeletalModelLoader::SetVertexBoneDataToDefault(Vertex& vertex)
    {
    }

    void SkeletalModelLoader::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
    {
    }
    glm::mat4 SkeletalModelLoader::aiMatrix4x4ToGlm(const aiMatrix4x4& from)
    {
        return glm::mat4();
    }
    aiMatrix4x4 SkeletalModelLoader::glmToAiMatrix4x4(const glm::mat4& from)
    {
        return aiMatrix4x4();
    }


    void SkeletalModelLoader::processMaterials(aiMesh* mesh, const aiScene* scene, Ref<Model> modelBuild, ImportModelData importOptions)
    {
        auto material = CreateRef<Material>();

        //COLOR DIFUSSE
        aiColor3D color(0.f, 0.f, 0.f);
        const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);

        material->materialName = mat->GetName().C_Str();

        material->albedoColor.r = color.r;
        material->albedoColor.g = color.g;
        material->albedoColor.b = color.b;

        // Agregamos la carga de la textura ALBEDO aquí
        aiString texturePath;


        if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();

            Ref<Texture> texture = libCore::TextureManager::LoadTexture(completePathTexture.c_str(), TEXTURE_TYPES::ALBEDO, 0);

            if (texture != nullptr)
            {
                material->albedoMap = texture;
            }
            else
            {
                std::cout << "Error cargando mapa diffuso, ponemos default_white" << std::endl;
                material->albedoMap = assetsManager.GetTexture("default_albedo");
            }
        }
        else
        {
            std::cout << "No hay mapa diffuso, ponemos default_white" << std::endl;
            material->albedoMap = assetsManager.GetTexture("default_albedo");
        }

        modelBuild->materials.push_back(material);
    }
}
