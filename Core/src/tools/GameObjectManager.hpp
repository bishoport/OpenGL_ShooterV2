//#pragma once
//#include "../LibCoreHeaders.h"
//#include "../Core/Material.h"
//#include "AssetsManager.h"
//#include "ModelLoader.h"
//#include "PrimitivesHelper.h"
//#include "../StraightSkeleton/Vector2d.h"
//
//
//namespace libCore
//{
//    class GameObjectManager
//    {
//
//    public:
//        static GameObjectManager& getInstance() {
//            static GameObjectManager instance;
//            return instance;
//        }
//        // Elimina los métodos de copia y asignación
//        GameObjectManager(GameObjectManager const&) = delete;
//        void operator=(GameObjectManager const&) = delete;
//
//    public:
//
//		//--CREADOR DE PREFABS
//		Ref<Model> CreatePrefabExternalModel(ImportModelData importModelData)
//		{
//			return libCore::ModelLoader::LoadModel(importModelData);
//		}
//		Ref<Model> CreatePrefabDot(const glm::vec3& pos, const glm::vec3& polygonColor)
//		{
//			auto modelBuild = CreateRef<Model>();
//			modelBuild->transform.position = pos;
//			modelBuild->name = "PRIMITIVE_DOT";
//			modelBuild->meshes.push_back(PrimitivesHelper::CreateDot());
//
//
//			//--DEFAULT_MATERIAL
//			auto material = CreateRef<Material>();
//			material->materialName = "default_material";
//
//			material->albedoColor = polygonColor;
//
//			material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
//
//			modelBuild->materials.push_back(material);
//
//			return modelBuild;
//		}
//		Ref<Model> CreatePrefabLine(const glm::vec3& point1, const glm::vec3& point2)
//		{
//			auto modelBuild = CreateRef<Model>();
//
//			modelBuild->name = "PRIMITIVE_LINE";
//			modelBuild->meshes.push_back(PrimitivesHelper::CreateLine(point1, point2));
//
//
//			//--DEFAULT_MATERIAL
//			auto material = CreateRef<Material>();
//			material->materialName = "default_material";
//
//			material->albedoColor.r = 1.0f;
//			material->albedoColor.g = 1.0f;
//			material->albedoColor.b = 1.0f;
//
//			material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
//			material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
//			material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
//			material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
//			material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");
//
//			modelBuild->materials.push_back(material);
//
//			return modelBuild;
//		}
//		Ref<Model> CreateTriangle(const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3)
//		{
//			auto modelBuild = CreateRef<Model>();
//
//
//			modelBuild->name = "PRIMIVITE_TRIANGLE";
//			modelBuild->meshes.push_back(PrimitivesHelper::CreateTriangle(pos1, pos2, pos3));
//
//
//			//--DEFAULT_MATERIAL
//			auto material = CreateRef<Material>();
//			material->materialName = "default_material";
//
//			material->albedoColor.r = 1.0f;
//			material->albedoColor.g = 1.0f;
//			material->albedoColor.b = 1.0f;
//
//			material->albedoMap = AssetsManager::GetInstance().GetTexture("checker");
//			material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
//			material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
//			material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
//			material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");
//
//			modelBuild->materials.push_back(material);
//
//			return modelBuild;
//		}
//		Ref<Model> CreatePrefabSphere(float radius, unsigned int sectorCount, unsigned int stackCount)
//		{
//			auto modelBuild = CreateRef<Model>();
//
//			modelBuild->name = "PRIMIVITE_SPHERE";
//			modelBuild->meshes.push_back(PrimitivesHelper::CreateSphere(0.01f, 6, 6));
//
//			//--DEFAULT_MATERIAL
//			auto material = CreateRef<Material>();
//			material->materialName = "default_material";
//
//			material->albedoColor.r = 1.0f;
//			material->albedoColor.g = 1.0f;
//			material->albedoColor.b = 1.0f;
//
//			material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
//			material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
//			material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
//			material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
//			material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");
//
//			modelBuild->materials.push_back(material);
//
//			return modelBuild;
//		}
//		Ref<Model> CreatePrefabCube(glm::vec3 position)
//		{
//			auto modelBuild = CreateRef<Model>();
//
//			modelBuild->name = "PRIMIVITE_CUBE";
//			modelBuild->meshes.push_back(PrimitivesHelper::CreateCube());
//			modelBuild->transform.position = position;
//
//			//--DEFAULT_MATERIAL
//			auto material = CreateRef<Material>();
//			material->materialName = "default_material";
//
//			material->albedoColor.r = 1.0f;
//			material->albedoColor.g = 1.0f;
//			material->albedoColor.b = 1.0f;
//
//			material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
//			material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
//			material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
//			material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
//			material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");
//
//			modelBuild->materials.push_back(material);
//
//			return modelBuild;
//		}
//		//Ref<Model> CreateRoof(const std::vector<Vector2d>& points, const std::vector<Vector2d>& holes)
//		//{
//		//	return roofGenerator->GenerateRoof(points, holes);
//		//}
//		// -------------------------------------------------
//		// -------------------------------------------------
//
//
//        
//  
//    private:
//        GameObjectManager() {};
//    };
//}
