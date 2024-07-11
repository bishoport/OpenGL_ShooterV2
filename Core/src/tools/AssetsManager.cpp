#include "AssetsManager.h"
#include "TextureManager.h"

namespace libCore
{
	AssetsManager& assetsManager = AssetsManager::GetInstance();

	void AssetsManager::LoadDefaultAssets()
	{
		std::string defaultAssetsPathTexture = "C:/Produccion/PROPIOS/OpenGL_DEV/OpenGLSandbox/Core/assets/textures/";

		Ref<Texture> defaultAlbedo = TextureManager::LoadTexture((defaultAssetsPathTexture).c_str(), "default_white.jpg", TEXTURE_TYPES::ALBEDO, 0);
		SetTexture("default_albedo", defaultAlbedo);

		Ref<Texture> defaultNormal = TextureManager::LoadTexture((defaultAssetsPathTexture ).c_str(), "default_normal.jpg", TEXTURE_TYPES::NORMAL, 1);
		SetTexture("default_normal", defaultNormal);

		Ref<Texture> defaultMetallic = TextureManager::LoadTexture((defaultAssetsPathTexture).c_str(), "default_black.jpg", TEXTURE_TYPES::METALLIC, 2);
		SetTexture("default_metallic", defaultMetallic);

		Ref<Texture> defaultRoughness = TextureManager::LoadTexture((defaultAssetsPathTexture).c_str(), "default_black.jpg", TEXTURE_TYPES::ROUGHNESS, 3);
		SetTexture("default_roughness", defaultRoughness);

		Ref<Texture> defaultAO = TextureManager::LoadTexture((defaultAssetsPathTexture ).c_str(), "default_white.jpg", TEXTURE_TYPES::AO, 4);
		SetTexture("default_ao", defaultAO);


		Ref<Texture> checker = TextureManager::LoadTexture((defaultAssetsPathTexture).c_str(), "checker.jpg", TEXTURE_TYPES::ALBEDO, 0);
		SetTexture("checker", checker);

		

		//PARA EL TEJADO
		//Ref<Texture> defaultRoof = TextureManager::LoadTexture((defaultAssetsPathTexture + "roof.jpg").c_str(), TEXTURE_TYPES::ALBEDO, 0);
		//SetTexture("default_roof", defaultRoof);
		//
		//Ref<Texture> wallRoof = TextureManager::LoadTexture((defaultAssetsPathTexture + "carpet.png").c_str(), TEXTURE_TYPES::ALBEDO, 0);
		//SetTexture("default_wall", wallRoof);

	}

	// Método para obtener una textura
	Ref<Texture> AssetsManager::GetTexture(const std::string& name) {
		auto it = loadedTextures.find(name);
		if (it != loadedTextures.end()) {
			return it->second;
		}
		else {
			//std::cout << "No existe " << name << std::endl;
			return nullptr;
		}
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