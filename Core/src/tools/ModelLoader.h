#pragma once

#include "../LibCoreHeaders.h"

#include "../Core/Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Core/Material.h"
#include "../Core/Light.hpp"



namespace libCore
{
	// Struct para las opciones de importación
	struct ImportModelData {
		std::string filePath = "";
		std::string fileName = "";
		int modelID = 0;
		bool invertUV = false;
		bool rotate90 = false;
		bool skeletal = false;
		bool useCustomTransform = true;
		bool processLights = false;
		float globalScaleFactor = 1.0f; // Ajusta según tus necesidades
	};


	struct ModelContainer
	{
		std::string name;
		std::vector<Ref<Model>> models;
		bool skeletal = false;

		std::vector<Ref<Light>> lights;

		void Draw(const std::string& shader)
		{
			for (auto& model : models)
			{
				model->Draw(shader);
			}
		}

		Ref<Model> UnifyMeshes() {
			auto unifiedModel = CreateRef<Model>();

			for (const auto& model : models) {
				unifiedModel->meshes.insert(unifiedModel->meshes.end(), model->meshes.begin(), model->meshes.end());
				// Unir los mapas de huesos.
				for (const auto& [boneName, boneInfo] : model->GetBoneInfoMap()) {
					if (unifiedModel->GetBoneInfoMap().find(boneName) == unifiedModel->GetBoneInfoMap().end()) {
						unifiedModel->GetBoneInfoMap()[boneName] = boneInfo;
					}
				}
				unifiedModel->GetBoneCount() += model->GetBoneCount();
			}
			return unifiedModel;
		}
	};









	class ModelLoader 
	{
	public:
		static Ref<ModelContainer> LoadModel(ImportModelData importOptions);

	private:

		//Standard
		static void processNode(aiNode* node, const aiScene* scene, Ref<ModelContainer> modelContainer, aiMatrix4x4 _nodeTransform, ImportModelData importOptions);
		static void processMesh(aiMesh* mesh, const aiScene* scene, Ref<Model> modelBuild, aiMatrix4x4 finalTransform, ImportModelData importOptions, int meshIndex);
		static void processMaterials(aiMesh* mesh, const aiScene* scene, Ref<Model> modelBuild, ImportModelData importOptions);

		//Skeletal
		static void processSkeletalMesh(aiMesh* mesh, const aiScene* scene, Ref<Model> modelBuild, aiMatrix4x4 finalTransform, ImportModelData importOptions, int meshIndex);
		static void SetVertexBoneDataToDefault(Vertex& vertex);
		static void ExtractBoneWeightForVertices(Ref<Model> modelBuild, aiMesh* mesh, const aiScene* scene, int meshIndex);
		static void SetVertexBoneData(Vertex& vertex, int boneID, float weight);

		//Process Features
		static void processLights(const aiScene* scene, Ref<ModelContainer> modelContainer);

		//Tools
		static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);
		static aiMatrix4x4 glmToAiMatrix4x4(const glm::mat4& from);
	};
}