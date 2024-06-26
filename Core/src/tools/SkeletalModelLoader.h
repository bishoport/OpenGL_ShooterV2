#pragma once
#include "../Core/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Core/Material.h"
#include "ModelLoader.h"

namespace libCore
{
	class SkeletalModelLoader
	{
	public:
		static Ref<ModelContainer> LoadModel(ImportModelData importOptions);

	private:
		static void processNode(aiNode* node, const aiScene* scene, Ref<ModelContainer> modelContainer, aiMatrix4x4 _nodeTransform, ImportModelData importOptions);
		static void processNodeRecursive(aiNode* node, const aiScene* scene, Ref<Model> combinedModel, Ref<Mesh> combinedMesh, aiMatrix4x4 _nodeTransform, ImportModelData importOptions);
		static void processMaterials(aiMesh* mesh, const aiScene* scene, Ref<Model> modelBuild, ImportModelData importOptions);

		//Skeletal
		static void processMesh(aiMesh* mesh, const aiScene* scene, Ref<Model> combinedModel, Ref<Mesh> combinedMesh, glm::mat4 finalTransform, ImportModelData importOptions, int meshIndex);
		static void SetVertexBoneDataToDefault(Vertex& vertex);
		static void ExtractBoneWeightForVertices(Ref<Model> combinedModel, Ref<Mesh> combinedMesh, aiMesh* mesh, unsigned int baseIndex);
		static void SetVertexBoneData(Vertex& vertex, int boneID, float weight);

		//tools
		static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);
		static aiMatrix4x4 glmToAiMatrix4x4(const glm::mat4& from);
	};
}