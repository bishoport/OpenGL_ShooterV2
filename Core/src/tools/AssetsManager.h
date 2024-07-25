#pragma once

#include "../LibCoreHeaders.h"
#include "../Core/Texture.h"

namespace libCore
{
	class AssetsManager
	{
	public:

		// Método para obtener la instancia de la clase
		static AssetsManager& GetInstance() {
			static AssetsManager instance;
			return instance;
		}
		
		std::string defaultAssetsPathTexture = "C:/Produccion/PROPIOS/OpenGL_DEV/OpenGLSandbox/Core/assets/textures/";

		void UnloadTexture(const std::string& name);
		void LoadDefaultAssets();
		Ref<Texture> LoadTextureAsset(const std::string& key, const char* directoryPath, const char* fileName, TEXTURE_TYPES type);
		Ref<Texture> GetTexture(const std::string& name);
		void SetTexture(const std::string& name, const Ref<Texture>& texture);
		const std::unordered_map<std::string, Ref<Texture>>& GetAllTextures() const;
		std::size_t GetNumberOfTextures() const;
	
	private:
		// Constructor privado
		AssetsManager() {}

		std::unordered_map<std::string, Ref<Texture>> loadedTextures;

		// Eliminar la posibilidad de copiar o asignar instancias
		AssetsManager(const AssetsManager&) = delete;
		AssetsManager& operator=(const AssetsManager&) = delete;
	};
}