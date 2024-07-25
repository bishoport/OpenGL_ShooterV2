#include "AssetsManager.h"
#include "TextureManager.h"

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


	// Método para obtener una textura
	Ref<Texture> AssetsManager::GetTexture(const std::string& name) {
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

	// Método para eliminar una textura
	void AssetsManager::UnloadTexture(const std::string& name) {
		loadedTextures.erase(name);
	}
}