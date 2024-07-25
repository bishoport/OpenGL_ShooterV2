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
	};
}
