#pragma once

#include "../LibCoreHeaders.h"
//

#include "../Core/Texture.h"
#include "../Core/Model.h"


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

		//--TEXTURES
		std::string defaultAssetsPathTexture = "C:/Produccion/PROPIOS/OpenGL_DEV/OpenGLSandbox/Core/assets/textures/";
		void UnloadTexture(const std::string& name);
		void LoadDefaultAssets();
		Ref<Texture> LoadTextureAsset(const std::string& key, const char* directoryPath, const char* fileName, TEXTURE_TYPES type);
		Ref<Texture> GetTexture(const std::string& name);
		void SetTexture(const std::string& name, const Ref<Texture>& texture);
		const std::unordered_map<std::string, Ref<Texture>>& GetAllTextures() const;
		std::size_t GetNumberOfTextures() const;
		//---------------------------------------------------------------------------------------------------------------------------


		//--MODELS
		void LoadModelAsset(ImportModelData importModelData);
		Ref<Model> GetModel(const std::string& name);
		const std::unordered_map<std::string, Ref<Model>>& GetAllModels() const;
		//---------------------------------------------------------------------------------------------------------------------------


	private:
		// Constructor privado
		AssetsManager() {}

		//-CARGADOS EN MEMORIA
		std::unordered_map<std::string, Ref<Texture>> loadedTextures;
		std::unordered_map<std::string, Ref<Model>>   loadedModels;


		// Eliminar la posibilidad de copiar o asignar instancias
		AssetsManager(const AssetsManager&) = delete;
		AssetsManager& operator=(const AssetsManager&) = delete;
	};
}
