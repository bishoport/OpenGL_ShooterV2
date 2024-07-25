#include "AssetsManager.h"
#include "TextureManager.h"
#include "ModelLoader.h"

namespace libCore
{
	void AssetsManager::LoadDefaultAssets()
	{
		LoadTextureAsset("default_albedo",    (defaultAssetsPathTexture).c_str(), "default_albedo.jpg",    TEXTURE_TYPES::ALBEDO);
		LoadTextureAsset("default_normal",    (defaultAssetsPathTexture).c_str(), "default_normal.jpg",    TEXTURE_TYPES::NORMAL);
		LoadTextureAsset("default_metallic",  (defaultAssetsPathTexture).c_str(), "default_metallic.jpg",  TEXTURE_TYPES::METALLIC);
		LoadTextureAsset("default_roughness", (defaultAssetsPathTexture).c_str(), "default_roughness.jpg", TEXTURE_TYPES::ROUGHNESS);
		LoadTextureAsset("default_ao",        (defaultAssetsPathTexture).c_str(), "default_ao.jpg",        TEXTURE_TYPES::AO);
		LoadTextureAsset("checker",           (defaultAssetsPathTexture).c_str(), "checker.jpg",           TEXTURE_TYPES::ALBEDO);
	}


	//--TEXTURES
	Ref<Texture> AssetsManager::LoadTextureAsset(const std::string &key, const char* directoryPath, const char* fileName, TEXTURE_TYPES type)
	{
		int slot = 0;
		if (type      == TEXTURE_TYPES::ALBEDO)    slot = 0;
		else if (type == TEXTURE_TYPES::NORMAL)    slot = 1;
		else if (type == TEXTURE_TYPES::METALLIC)  slot = 2;
		else if (type == TEXTURE_TYPES::ROUGHNESS) slot = 3;
		else if (type == TEXTURE_TYPES::AO)        slot = 4;

		SetTexture(key, TextureManager::getInstance().LoadTexture(directoryPath, fileName, type, slot));

		return GetTexture(key);
	}
	Ref<Texture> AssetsManager::GetTexture(const std::string& name) { // Método para obtener una textura
		auto it = loadedTextures.find(name);
		if (it != loadedTextures.end()) {
			return it->second;
		}
		else {
			std::cout << "No existe " << name << std::endl;
			return nullptr;
		}
	}
	const std::unordered_map<std::string, Ref<Texture>>& AssetsManager::GetAllTextures() const {
		return loadedTextures;
	}
	std::size_t AssetsManager::GetNumberOfTextures() const {
		return loadedTextures.size();
	}
	void AssetsManager::SetTexture(const std::string& name, const Ref<Texture>& texture)
	{
		loadedTextures[name] = texture;
	}
	void AssetsManager::UnloadTexture(const std::string& name) {
		loadedTextures.erase(name);
	}
	//---------------------------------------------------------------------------------------------------------------------------

	//--MODELS
	void AssetsManager::LoadModelAsset(ImportModelData importModelData)
	{
		Ref<Model> model = ModelLoader::LoadModel(importModelData);
	
		if (model != nullptr)
		{
			std::string key = importModelData.filePath + importModelData.fileName;
			loadedModels[key] = model;
		}
		else 
		{
			std::cout << "Error Loading MODEL" << std::endl;
		}
		
	}
	Ref<Model> AssetsManager::GetModel(const std::string& name) {
		auto it = loadedModels.find(name);
		if (it != loadedModels.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}
	
	const std::unordered_map<std::string, Ref<Model>>& AssetsManager::GetAllModels() const {
		return loadedModels;
	}
	//---------------------------------------------------------------------------------------------------------------------------


	//--MATERIALS
	void AssetsManager::CreateDefaultMaterial()
	{
		auto material = CreateRef<Material>("default_material");
		addMaterial(material);
	}
	void AssetsManager::CreateEmptyMaterial(const std::string& matName)
	{
		Ref <Material> material = CreateRef<Material>(matName);
		addMaterial(material);
	}
	Ref<Material> AssetsManager::addMaterial(Ref<Material> materialData)
	{
		auto result = loadedMaterials.emplace(materialData->materialName, materialData);

		if (result.second) {
			std::cout << "Added new material: " << materialData->materialName << std::endl;
			return materialData;
		}
		else {
			// El material ya existía, devuelve el material existente.
			return result.first->second;
		}
	}
	Ref<Material> AssetsManager::getMaterial(const std::string& key)
	{
		auto it = loadedMaterials.find(key);
		if (it != loadedMaterials.end()) {
			return it->second;
		}
		std::cout << key << " material does exist" << std::endl;
		return nullptr; // O manejar el error como prefieras
	}
	bool AssetsManager::removeMaterial(const std::string& key)
	{
		return loadedMaterials.erase(key) > 0;
	}
	const std::unordered_map<std::string, Ref<Material>>& AssetsManager::GetAllMaterials() const
	{
		return loadedMaterials;
	}
	//---------------------------------------------------------------------------------------------------------------------------
}