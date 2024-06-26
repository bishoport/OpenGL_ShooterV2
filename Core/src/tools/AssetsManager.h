#pragma once

#include "../LibCoreHeaders.h"
#include "../Core/Texture.h"

namespace libCore
{
	class AssetsManager
	{
	public:
		// M�todo para obtener la instancia de la clase
		static AssetsManager& GetInstance() {
			static AssetsManager instance;
			return instance;
		}
		
		void UnloadTexture(const std::string& name);
		void LoadDefaultAssets();
		Ref<Texture> GetTexture(const std::string& name);
		void SetTexture(const std::string& name, const Ref<Texture>& texture);

	private:

		// Constructor privado
		AssetsManager() {}

		std::unordered_map<std::string, Ref<Texture>> loadedTextures;

		// Eliminar la posibilidad de copiar o asignar instancias
		AssetsManager(const AssetsManager&) = delete;
		AssetsManager& operator=(const AssetsManager&) = delete;
	};

	// Declaraci�n externa de una referencia a AssetsManager
	extern AssetsManager& assetsManager;
}