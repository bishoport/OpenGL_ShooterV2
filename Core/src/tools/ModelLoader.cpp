#include "ModelLoader.h"
#include "TextureManager.h"
#include "AssimpGlmHelpers.h"
#include "../Core/Light.hpp"
#include "LightsManager.hpp"

namespace libCore
{

    Ref<ModelContainer> ModelLoader::LoadModel(ImportModelData importOptions)
    {
        auto modelContainer = CreateRef<ModelContainer>();
             modelContainer->skeletal = importOptions.skeletal;

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
            //throw std::runtime_error("Failed to load model");
            return modelContainer;
        }

        aiMatrix4x4 nodeTransform = scene->mRootNode->mTransformation;
        modelContainer->name = importOptions.fileName;

        ModelLoader::processNode(scene->mRootNode, scene, modelContainer, nodeTransform, importOptions);

        if (importOptions.processLights == true)
        {
            ModelLoader::processLights(scene, modelContainer);
        }
        
        return modelContainer;
    }


    //------------------------------------STANDARD MODELS---------------------------
    void ModelLoader::processNode(aiNode* node, const aiScene* scene, Ref<ModelContainer> modelContainer, aiMatrix4x4 _nodeTransform, ImportModelData importOptions)
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
            auto modelBuild = CreateRef<Model>();

            if (modelContainer->skeletal == false)
            {
                processMesh(mesh, scene, modelBuild, finalTransform, importOptions, i);
            }
            else
            {
                processSkeletalMesh(mesh, scene, modelBuild, finalTransform, importOptions, i);
            }
            
            processMaterials(mesh, scene, modelBuild, importOptions);

            modelContainer->models.push_back(modelBuild);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ModelLoader::processNode(node->mChildren[i], scene, modelContainer, finalTransform, importOptions); // Asegúrate de pasar `finalTransform`
        }
    }
    void ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, Ref<Model> modelBuild, aiMatrix4x4 finalTransform, ImportModelData importOptions, int meshIndex)
    {
        auto meshBuild = CreateRef<Mesh>();

        modelBuild->transform.position = glm::vec3(finalTransform.a4, finalTransform.b4, finalTransform.c4);

        //Reset de la posicion original para que nos devuelva la matriz en la posicion 0,0,0
        finalTransform.a4 = 0.0;
        finalTransform.b4 = 0.0;
        finalTransform.c4 = 0.0;

        // Cargando los datos de los vértices y los índices
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
        meshBuild->meshName = meshNameBase + std::to_string(importOptions.modelID);

        modelBuild->meshes.push_back(meshBuild);

        if (importOptions.skeletal == true)//--PROCCESS BONES
        {
            ExtractBoneWeightForVertices(modelBuild, mesh, scene, meshIndex);
        }

        meshBuild->SetupMesh();
    }
    void ModelLoader::processMaterials(aiMesh* mesh, const aiScene* scene, Ref<Model> modelBuild, ImportModelData importOptions)
    {
        auto material = CreateRef<Material>();


        //COLOR DIFUSSE
        aiColor3D color(1.f, 1.f, 1.f);
        const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);

        material->materialName = mat->GetName().C_Str();

        material->albedoColor.r = color.r;
        material->albedoColor.g = color.g;
        material->albedoColor.b = color.b;


        // Agregamos la carga de la textura ALBEDO aquí
        aiString texturePath;


        //--ALBEDO
        if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {

            material->albedoMap = assetsManager.GetTexture("default_albedo");
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();
            //std::cout << "Loading Texture " << completePathTexture << std::endl;
            Ref<Texture> texture = libCore::TextureManager::LoadTexture(completePathTexture.c_str(), TEXTURE_TYPES::ALBEDO, 0);

            if (texture != nullptr) 
            {
                material->albedoMap = texture;
            }
            else
            {
                //std::cout << "Error cargando mapa diffuso, ponemos default_white" << std::endl;
                material->albedoMap = assetsManager.GetTexture("default_albedo");
            }
        }
        else
        {
            //std::cout << "No hay mapa diffuso, ponemos default_white" << std::endl;
            material->albedoMap = assetsManager.GetTexture("default_albedo");
        }
        //-------------------

        //--NORMAL
        if (mat->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();

            Ref<Texture> texture = libCore::TextureManager::LoadTexture(completePathTexture.c_str(), TEXTURE_TYPES::NORMAL, 1);

            if (texture != nullptr)
            {
                material->normalMap = texture;
            }
            else
            {
                //std::cout << "Error cargando mapa Normal, ponemos default_normal" << std::endl;
                material->normalMap = assetsManager.GetTexture("default_normal");
            }
        }
        else
        {
            //std::cout << "No hay mapa Normal, ponemos default_normal" << std::endl;
            material->normalMap = assetsManager.GetTexture("default_normal");
        }
        //-------------------


        //--METALLIC
        if (mat->GetTexture(aiTextureType_METALNESS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();

            Ref<Texture> texture = libCore::TextureManager::LoadTexture(completePathTexture.c_str(), TEXTURE_TYPES::METALLIC, 2);

            if (texture != nullptr)
            {
                material->metallicMap = texture;
            }
            else
            {
                //std::cout << "Error cargando mapa Metallic, ponemos default_black" << std::endl;
                material->metallicMap = assetsManager.GetTexture("default_metallic");
            }
        }
        else
        {
            //std::cout << "No hay mapa Metallic, ponemos default_black" << std::endl;
            material->metallicMap = assetsManager.GetTexture("default_metallic");
        }
        //-------------------


        //--ROUGHNESS
        if (mat->GetTexture(aiTextureType_SHININESS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();

            Ref<Texture> texture = libCore::TextureManager::LoadTexture(completePathTexture.c_str(), TEXTURE_TYPES::ROUGHNESS, 3);

            if (texture != nullptr)
            {
                material->roughnessMap = texture;
            }
            else
            {
                //std::cout << "Error cargando mapa ROUGHTNESS, ponemos default_black" << std::endl;
                material->roughnessMap = assetsManager.GetTexture("default_roughness");
            }
        }
        else
        {
            //std::cout << "No hay mapa ROUGHTNESS, ponemos default_black" << std::endl;
            material->roughnessMap = assetsManager.GetTexture("default_roughness");
        }
        //-------------------



        //--AO
        if (mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath) == AI_SUCCESS)
        {
            std::string completePathTexture = importOptions.filePath + texturePath.C_Str();

            Ref<Texture> texture = libCore::TextureManager::LoadTexture(completePathTexture.c_str(), TEXTURE_TYPES::AO, 4);

            if (texture != nullptr)
            {
                material->aOMap = texture;
            }
            else
            {
                //std::cout << "Error cargando mapa AO, ponemos default_withe" << std::endl;
                material->aOMap = assetsManager.GetTexture("default_ao");
            }
        }
        else
        {
            //std::cout << "No hay mapa AO, ponemos default_withe" << std::endl;
            material->aOMap = assetsManager.GetTexture("default_ao");
        }
        //-------------------

        modelBuild->materials.push_back(material);
    }
    //-----------------------------------------------------------------------
 

    //-----------------------------------FEATURES PROCESS---------------------------
    void ModelLoader::processLights(const aiScene* scene, Ref<ModelContainer> modelContainer)
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
                //lightType = POINT;
            }
            else if (lightName.find("Spot") != std::string::npos) {
                libCore::LightsManager::CreateLight(true, libCore::LightType::SPOT, glm::vec3(position.x, position.y, position.z));
                //lightType = SPOT;
            }
            else if (lightName.find("Area") != std::string::npos) {
                libCore::LightsManager::CreateLight(true, libCore::LightType::AREA, glm::vec3(position.x, position.y, position.z));
                //lightType = AREA;
            }
            else
            {
                std::cerr << "Unknown light type: " << lightName << std::endl;
                continue;
            }
        }
    }


    //------------------------------------SKELETAL MODELS---------------------------
    void ModelLoader::processSkeletalMesh(aiMesh* mesh, const aiScene* scene, Ref<Model> modelBuild, aiMatrix4x4 finalTransform, ImportModelData importOptions, int meshIndex)
    {
        auto meshBuild = CreateRef<Mesh>();

        modelBuild->transform.position = glm::vec3(finalTransform.a4, finalTransform.b4, finalTransform.c4);

        //Reset de la posicion original para que nos devuelva la matriz en la posicion 0,0,0
        finalTransform.a4 = 0.0;
        finalTransform.b4 = 0.0;
        finalTransform.c4 = 0.0;

        // Cargando los datos de los vértices y los índices
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
}
