#include "ModelLoader.h"
#include "TextureManager.h"
#include "AssimpGlmHelpers.h"
#include "../Core/Light.hpp"
#include "LightsManager.hpp"
#include "AssetsManager.h"
#include <filesystem>

namespace libCore
{
    namespace fs = std::filesystem;


    std::string getFileName(const std::string& path) {
        size_t lastSlash = path.find_last_of("\\/");
        if (lastSlash == std::string::npos) {
            return path; // No hay ning�n separador, toda la cadena es el nombre del archivo
        }
        return path.substr(lastSlash + 1);
    }

    Ref<Model> ModelLoader::LoadModel(ImportModelData importOptions)
    {
        auto modelParent = CreateRef<Model>();
             modelParent->skeletal = false;// importOptions.skeletal;

        Assimp::Importer importer;
        std::string completePath = importOptions.filePath + importOptions.fileName;

        unsigned int flags  = aiProcess_Triangulate;
                     flags |= aiProcess_CalcTangentSpace;
                     flags |= aiProcess_GenSmoothNormals;
                     flags |= aiProcess_ValidateDataStructure;
                     flags |= aiProcess_GenBoundingBoxes;

        if (importOptions.invertUV == true) flags |= aiProcess_FlipUVs;

        const aiScene* scene = importer.ReadFile(completePath, flags);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return nullptr;  //--> lo deolvemos vacio
        }



        aiMatrix4x4 nodeTransform = scene->mRootNode->mTransformation;
        modelParent->name = importOptions.fileName;

        ModelLoader::processNode(scene->mRootNode, scene, modelParent, nodeTransform, importOptions);

        if (importOptions.processLights == true)
        {
            ModelLoader::processLights(scene);
        }
        
        return modelParent;
    }


    //------------------------------------STANDARD MODELS---------------------------
    void ModelLoader::processNode(aiNode* node, const aiScene* scene, Ref<Model> modelParent, aiMatrix4x4 _nodeTransform, ImportModelData importOptions)
    {
        glm::mat4 glmNodeTransform = aiMatrix4x4ToGlm(_nodeTransform);
        glm::mat4 glmNodeTransformation = aiMatrix4x4ToGlm(node->mTransformation);

        float globalRotationDeg_X = 0.0f;
        if (importOptions.rotate90) globalRotationDeg_X = -90.0f;

        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(globalRotationDeg_X), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(importOptions.globalScaleFactor, importOptions.globalScaleFactor, importOptions.globalScaleFactor));
        glm::mat4 glmFinalTransform = rotationX * scaleMatrix * glmNodeTransform * glmNodeTransformation;
        aiMatrix4x4 finalTransform = glmToAiMatrix4x4(glmFinalTransform);


        for (unsigned int i = 0; i < node->mNumMeshes; i++) {

            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            auto modelChild = CreateRef<Model>();

            // Aqu� establecemos la relaci�n padre-hijo
            modelChild->modelParent = modelParent;

            // Asignar el nombre del nodo de Assimp al modelo
            modelChild->name = node->mName.C_Str();

            if (modelParent->skeletal == false)
            {
                processMesh(mesh, scene, modelChild, finalTransform, importOptions, i);
            }
            else
            {
                processSkeletalMesh(mesh, scene, modelChild, finalTransform, importOptions, i);
            }
            
            processMaterials(mesh, scene, modelChild, importOptions);

            modelParent->children.push_back(modelChild);
        }


        for (unsigned int i = 0; i < node->mNumChildren; i++) 
        {
            ModelLoader::processNode(node->mChildren[i], scene, modelParent, finalTransform, importOptions);
        }
    }
    void ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, Ref<Model> modelBuild, aiMatrix4x4 finalTransform, ImportModelData importOptions, int meshIndex)
    {
        auto meshBuild = CreateRef<Mesh>();

        modelBuild->transform->position = glm::vec3(finalTransform.a4, finalTransform.b4, finalTransform.c4);

        //Reset de la posicion original para que nos devuelva la matriz en la posicion 0,0,0
        finalTransform.a4 = 0.0;
        finalTransform.b4 = 0.0;
        finalTransform.c4 = 0.0;

        // Cargando los datos de los v�rtices y los �ndices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            if (importOptions.skeletal == true)
            {
                SetVertexBoneDataToDefault(vertex);
            }
            

            //--Vertex Position
            if (importOptions.useCustomTransform == true)
            {
                glm::vec4 posFixed = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                    mesh->mVertices[i].x,
                    mesh->mVertices[i].y,
                    mesh->mVertices[i].z,
                    1);

                vertex.position = glm::vec3(posFixed.x, posFixed.y, posFixed.z);
            }
            else
            {
                vertex.position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
            }
            //--------------------------------------------------------------


            //--Texture Coords
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texUV = vec;
            }
            else
            {
                vertex.texUV = glm::vec2(0.0f, 0.0f);
            }
            //--------------------------------------------------------------


            //--Vertex Normal
            if (importOptions.useCustomTransform == true)
            {
                glm::vec4 normFixed = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z,
                    1);

                vertex.normal = glm::vec3(normFixed.x, normFixed.y, normFixed.z);
            }
            else
            {
                vertex.normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);
            }
            //--------------------------------------------------------------


            //--Vertex Tangent
            if (mesh->mTangents) {
                glm::vec4 tangentFixed = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                    mesh->mTangents[i].x,
                    mesh->mTangents[i].y,
                    mesh->mTangents[i].z,
                    1.0);

                vertex.tangent = glm::vec3(tangentFixed.x, tangentFixed.y, tangentFixed.z);
            }
            else {
                vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            //--------------------------------------------------------------

            //--Vertex Bitangent
            if (mesh->mBitangents) {
                glm::vec4 bitangentFixed = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                    mesh->mBitangents[i].x,
                    mesh->mBitangents[i].y,
                    mesh->mBitangents[i].z,
                    1.0);

                vertex.bitangent = glm::vec3(bitangentFixed.x, bitangentFixed.y, bitangentFixed.z);
            }
            else {
                vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            //--------------------------------------------------------------

            meshBuild->vertices.push_back(vertex);
        }

        //-INDICES
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                meshBuild->indices.push_back(face.mIndices[j]);
            }
        }

        //-MESH ID
        std::string meshNameBase = mesh->mName.C_Str();
        meshNameBase.append(" id:");
        meshBuild->meshName = meshNameBase;

        modelBuild->meshes.push_back(meshBuild);

        if (importOptions.skeletal == true)//--PROCCESS BONES
        {
            ExtractBoneWeightForVertices(modelBuild, mesh, scene, meshIndex);
        }

        meshBuild->SetupMesh();
    }
    void ModelLoader::processMaterials(aiMesh* mesh, const aiScene* scene, Ref<Model> modelBuild, ImportModelData importOptions)
    {
        // Obtener el material del mesh
        const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        std::string materialName = mat->GetName().C_Str();

        // Verificar si el material ya existe en el MaterialManager
        auto existingMaterial = AssetsManager::GetInstance().getMaterial(materialName);
        
        Ref<Material> material;

        if (existingMaterial) {
            //std::cout << "Using existing material: " << materialName << std::endl;
            material = existingMaterial;
        }
        else {
            // Crear un nuevo material
            material = CreateRef<Material>(materialName);
            //material->materialName = materialName;

            // Obtener el color difuso del material
            aiColor3D color(1.f, 1.f, 1.f);
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            material->albedoColor = { color.r, color.g, color.b };

            aiString texturePath;

            //--ALBEDO
            if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
                std::string completePathTexture = texturePath.C_Str();
                std::cout << "Loading Texture: " << completePathTexture << std::endl;

                std::string directoryPath = fs::path(importOptions.filePath).parent_path().string();
                std::string fileName = fs::path(completePathTexture).filename().string();

                std::string key = materialName + "_ALBEDO";

                Ref<Texture> texture = AssetsManager::GetInstance().LoadTextureAsset(key, directoryPath.c_str(), fileName.c_str(), TEXTURE_TYPES::ALBEDO);
                //Ref<Texture> texture = libCore::TextureManager::LoadTexture(directoryPath.c_str(), fileName.c_str(), TEXTURE_TYPES::ALBEDO, 0);

                if (texture != nullptr) {
                    material->albedoMap = texture;
                }
                else {
                    material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
                }
            }
            else {
                material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
            }

            //--NORMAL
            if (mat->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS) {
                std::string completePathTexture = texturePath.C_Str();
                std::cout << "Loading Normal Map: " << completePathTexture << std::endl;

                std::string directoryPath = fs::path(importOptions.filePath).parent_path().string();
                std::string fileName = fs::path(completePathTexture).filename().string();

                std::string key = materialName + "_NORMAL";
                Ref<Texture> texture = AssetsManager::GetInstance().LoadTextureAsset(key, directoryPath.c_str(), fileName.c_str(), TEXTURE_TYPES::NORMAL);
                //Ref<Texture> texture = libCore::TextureManager::LoadTexture(directoryPath.c_str(), fileName.c_str(), TEXTURE_TYPES::NORMAL, 1);

                if (texture != nullptr) 
                {
                    material->normalMap = texture;
                }
                else 
                {
                    material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
                }
            }
            else {
                material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
            }

            //--METALLIC
            if (mat->GetTexture(aiTextureType_METALNESS, 0, &texturePath) == AI_SUCCESS) {
                std::string completePathTexture = texturePath.C_Str();
                std::cout << "Loading Metallic Map: " << completePathTexture << std::endl;

                std::string directoryPath = fs::path(importOptions.filePath).parent_path().string();
                std::string fileName = fs::path(completePathTexture).filename().string();

                std::string key = materialName + "_METALLIC";
                Ref<Texture> texture = AssetsManager::GetInstance().LoadTextureAsset(key, directoryPath.c_str(), fileName.c_str(), TEXTURE_TYPES::METALLIC);

                //Ref<Texture> texture = libCore::TextureManager::LoadTexture(directoryPath.c_str(), fileName.c_str(), TEXTURE_TYPES::METALLIC, 2);

                if (texture != nullptr) {
                    material->metallicMap = texture;
                }
                else {
                    material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
                }
            }
            else {
                material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
            }

            //--ROUGHNESS
            if (mat->GetTexture(aiTextureType_SHININESS, 0, &texturePath) == AI_SUCCESS) {
                std::string completePathTexture = texturePath.C_Str();
                std::cout << "Loading Roughness Map: " << completePathTexture << std::endl;

                std::string directoryPath = fs::path(importOptions.filePath).parent_path().string();
                std::string fileName = fs::path(completePathTexture).filename().string();

                std::string key = materialName + "_ROUGHNESS";
                Ref<Texture> texture = AssetsManager::GetInstance().LoadTextureAsset(key, directoryPath.c_str(), fileName.c_str(), TEXTURE_TYPES::ROUGHNESS);

                //Ref<Texture> texture = libCore::TextureManager::LoadTexture(directoryPath.c_str(), fileName.c_str(), TEXTURE_TYPES::ROUGHNESS, 3);

                if (texture != nullptr) {
                    material->roughnessMap = texture;
                }
                else {
                    material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
                }
            }
            else {
                material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
            }

            // A�adir el material al MaterialManager
            AssetsManager::GetInstance().addMaterial(material);
            
        }

        // A�adir el material al modelo
        modelBuild->materials.push_back(material);
    }
    //------------------------------------------------------------------------------


    //-------------------------------------TOOLS---------------------------
    glm::mat4 ModelLoader::aiMatrix4x4ToGlm(const aiMatrix4x4& from)
    {
        glm::mat4 to;

        // Transponer y convertir a glm
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

        return to;
    }
    aiMatrix4x4 ModelLoader::glmToAiMatrix4x4(const glm::mat4& from)
    {
        aiMatrix4x4 to;

        // Transponer y convertir a Assimp
        to.a1 = from[0][0]; to.a2 = from[1][0]; to.a3 = from[2][0]; to.a4 = from[3][0];
        to.b1 = from[0][1]; to.b2 = from[1][1]; to.b3 = from[2][1]; to.b4 = from[3][1];
        to.c1 = from[0][2]; to.c2 = from[1][2]; to.c3 = from[2][2]; to.c4 = from[3][2];
        to.d1 = from[0][3]; to.d2 = from[1][3]; to.d3 = from[2][3]; to.d4 = from[3][3];

        return to;
    }
    //-----------------------------------------------------------------------


    //------------------------------------SKELETAL MODELS---------------------------
    void ModelLoader::processSkeletalMesh(aiMesh* mesh, const aiScene* scene, Ref<Model> modelBuild, aiMatrix4x4 finalTransform, ImportModelData importOptions, int meshIndex)
    {
        auto meshBuild = CreateRef<Mesh>();

        modelBuild->transform->position = glm::vec3(finalTransform.a4, finalTransform.b4, finalTransform.c4);

        //Reset de la posicion original para que nos devuelva la matriz en la posicion 0,0,0
        finalTransform.a4 = 0.0;
        finalTransform.b4 = 0.0;
        finalTransform.c4 = 0.0;

        // Cargando los datos de los v�rtices y los �ndices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            SetVertexBoneDataToDefault(vertex);

            //--Vertex Position
            vertex.position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
            //--------------------------------------------------------------


            //--Texture Coords
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texUV = vec;
            }
            else
            {
                vertex.texUV = glm::vec2(0.0f, 0.0f);
            }
            //--------------------------------------------------------------


            //--Vertex Normal
            vertex.normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);
            //--------------------------------------------------------------


            //--Vertex Tangent
            if (mesh->mTangents) {
                glm::vec4 tangentFixed = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                    mesh->mTangents[i].x,
                    mesh->mTangents[i].y,
                    mesh->mTangents[i].z,
                    1.0);

                vertex.tangent = glm::vec3(tangentFixed.x, tangentFixed.y, tangentFixed.z);
            }
            else {
                vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            //--------------------------------------------------------------

            //--Vertex Bitangent
            if (mesh->mBitangents) {
                glm::vec4 bitangentFixed = aiMatrix4x4ToGlm(finalTransform) * glm::vec4(
                    mesh->mBitangents[i].x,
                    mesh->mBitangents[i].y,
                    mesh->mBitangents[i].z,
                    1.0);

                vertex.bitangent = glm::vec3(bitangentFixed.x, bitangentFixed.y, bitangentFixed.z);
            }
            else {
                vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            //--------------------------------------------------------------

            meshBuild->vertices.push_back(vertex);
        }

        //-INDICES
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                meshBuild->indices.push_back(face.mIndices[j]);
            }
        }

        //-MESH ID
        std::string meshNameBase = mesh->mName.C_Str();
        meshNameBase.append(" id:");
        meshBuild->meshName = meshNameBase + std::to_string(importOptions.modelID);
        modelBuild->meshes.push_back(meshBuild);

        if (importOptions.skeletal == true)//--PROCCESS BONES
        {
            ExtractBoneWeightForVertices(modelBuild, mesh, scene, meshIndex);
        }

        meshBuild->SetupMesh();
    }
    void ModelLoader::SetVertexBoneDataToDefault(Vertex& vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            //vertex.m_BoneIDs[i] = -1;
            //vertex.m_Weights[i] = 0.0f;
        }
    }
    void ModelLoader::ExtractBoneWeightForVertices(Ref<Model> modelBuild, aiMesh* mesh, const aiScene* scene, int meshIndex)
    {

        auto& boneInfoMap = modelBuild->m_BoneInfoMap;
        int& boneCount = modelBuild->m_BoneCounter;
        auto& targetMesh = modelBuild->meshes[meshIndex];

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
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

            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;

                assert(vertexId < targetMesh->vertices.size());
                SetVertexBoneData(targetMesh->vertices[vertexId], boneID, weight);
            }
        }

        /*auto& boneInfoMap = modelBuild->m_BoneInfoMap;
        int& boneCount = modelBuild->m_BoneCounter;

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= modelBuild->meshes[0]->vertices.size());
                SetVertexBoneData(modelBuild->meshes[0]->vertices[vertexId], boneID, weight);
            }
        }*/
    }
    void ModelLoader::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            //if (vertex.m_BoneIDs[i] < 0)
            //{
            //    vertex.m_Weights[i] = weight;
            //    vertex.m_BoneIDs[i] = boneID;
            //    break;
            //}
        }
    }
    //-----------------------------------------------------------------------

    //-----------------------------------FEATURES PROCESS---------------------------
    void ModelLoader::processLights(const aiScene* scene)
    {
        for (unsigned int i = 0; i < scene->mNumLights; i++) {

            aiLight* ai_light = scene->mLights[i];

            // Find the node that corresponds to the light
            aiNode* lightNode = scene->mRootNode->FindNode(ai_light->mName);

            // Get the transformation matrix of the node
            aiMatrix4x4 transform = lightNode->mTransformation;

            // Apply parent node transformations
            aiNode* parentNode = lightNode->mParent;

            while (parentNode != nullptr) {
                transform = parentNode->mTransformation * transform;
                parentNode = parentNode->mParent;
            }

            // Transform the light position
            aiVector3D position = ai_light->mPosition;
            position *= transform;


            //LightType lightType;
            std::string lightName(ai_light->mName.C_Str());


            if (lightName.find("Point") != std::string::npos) {
                libCore::LightsManager::CreateLight(true, libCore::LightType::POINT, glm::vec3(position.x, position.z, position.y));
            }
            else if (lightName.find("Spot") != std::string::npos) {
                libCore::LightsManager::CreateLight(true, libCore::LightType::SPOT, glm::vec3(position.x, position.y, position.z));
            }
            else if (lightName.find("Area") != std::string::npos) {
                libCore::LightsManager::CreateLight(true, libCore::LightType::AREA, glm::vec3(position.x, position.y, position.z));
            }
            else
            {
                std::cerr << "Unknown light type: " << lightName << std::endl;
                continue;
            }
        }
    }
    //------------------------------------------------------------------------------
    
}
