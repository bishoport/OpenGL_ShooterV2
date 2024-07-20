#pragma once
#include <filesystem>
#include <glad/glad.h>
#include <fstream>
#include <functional>
#include "../tools/ModelLoader.h"
#include "../ECS/EntityManager.hpp"


namespace libCore {


	class AssetsPanel
	{
	public:

		AssetsPanel();


		void OnImGuiRender();

		// Función para establecer el delegado
		void SetDelegate(std::function<void(ImportModelData importOptions)> delegado) {
			//getModelPathFromAssetsDelegate = delegado;
		}



	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_DirectoryEntry;

		//Icons
		GLuint iconFolder;
		GLuint iconModel;
		GLuint iconImage;

		ImportModelData importOptions;
		bool isDialogOpen = false;

		std::filesystem::path s_AssetPath = "assets";


		//callbacks:
		std::function<void(ImportModelData importOptions)> getModelPathFromAssetsDelegate;

		// Función que utiliza el delegado
		void UsarDelegado(const ImportModelData importOptions) 
		{
			EntityManager::GetInstance().CreateExternalModelGameObject(importOptions);


			//if (getModelPathFromAssetsDelegate) {  // Verifica que el delegado haya sido establecido
			//	getModelPathFromAssetsDelegate(importOptions);
			//}
		}
	};
}
