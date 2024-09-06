#include "AssetsPanel.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <iostream>
#include "../Scripting/LuaManager.h"

namespace libCore {

	AssetsPanel::AssetsPanel()
		: m_BaseDirectory{},     // Inicializaci�n predeterminada
		  m_CurrentDirectory{},  // Inicializaci�n predeterminada
		  m_DirectoryEntry{},    // Inicializaci�n predeterminada
		  iconFolder{ 0 },       // Inicializaci�n predeterminada (0 es un valor com�n para inicializar texturas OpenGL)
		  iconModel{ 0 },        // Inicializaci�n predeterminada
		  iconImage{ 0 },        // Inicializaci�n predeterminada
		  importOptions{},       // Inicializaci�n predeterminada (llama al constructor por defecto de ImportOptions)
		  isDialogOpen{ false }  // Inicializaci�n predeterminada
	{
		m_CurrentDirectory = s_AssetPath;

		//std::cout << "m_CurrentDirectory-> " << m_CurrentDirectory << std::endl;

		//iconFolder = GLCore::Utils::ImageLoader::loadIcon("assets/default/icons/folder_icon.png");
		//iconModel = GLCore::Utils::ImageLoader::loadIcon("assets/default/icons/model_icon.png");
		//iconImage = GLCore::Utils::ImageLoader::loadIcon("assets/default/icons/picture_icon.png");
	}



	void AssetsPanel::OnImGuiRender()
	{
		ImGui::Begin("Assets", nullptr);

		ImGui::BeginChild("FileRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

		if (m_CurrentDirectory != std::filesystem::path(s_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 0.0f;
		static float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = thumbnailSize < 40 ? 1 : (int)(panelWidth / cellSize);

		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);


		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetPath);
			std::string filenameString = path.filename().string();

			ImGui::PushID(filenameString.c_str());

			GLuint iconTexture = 0; // Reemplazar con la ID de tu textura real

			if (directoryEntry.is_directory())
			{
				iconTexture = iconFolder; // Asumiendo que folderTexture es la ID de tu textura para carpetas
			}
			else if (path.extension() == ".fbx" || path.extension() == ".obj" || path.extension() == ".gltf")
			{
				iconTexture = iconModel; // Asumiendo que modelTexture es la ID de tu textura para modelos
			}
			else if (path.extension() == ".png" || path.extension() == ".jpg")
			{
				iconTexture = iconImage; // Asumiendo que imageTexture es la ID de tu textura para im�genes
			}
			else if (path.extension() == ".lua")
			{
				iconTexture = iconImage; // Asumiendo que imageTexture es la ID de tu textura para im�genes
			}



			if (iconTexture != 0)
			{
				ImGui::ImageButton((ImTextureID)(intptr_t)iconTexture, ImVec2(thumbnailSize - 15, thumbnailSize - 15));
			}
			else
			{
				ImGui::Button(filenameString.c_str(), { thumbnailSize, thumbnailSize });
			}


			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				if (directoryEntry.is_directory())
				{
					m_CurrentDirectory /= path.filename();
				}
				else if (path.extension() == ".yaml" || path.extension() == ".yml")
				{
					// Aqu� llamas a la funci�n para deserializar la escena
					std::string sceneName = path.stem().string(); // Obt�n el nombre del archivo sin extensi�n
					EngineOpenGL::GetInstance().currentScene->DeserializeScene(sceneName);
				}
				else if (path.extension() == ".lua")
				{
					std::string scriptName = path.stem().string();
					LuaManager::GetInstance().LoadLuaFile(scriptName, path.string());
				}
				else
				{
					isDialogOpen = true;  // Abre el di�logo
					m_DirectoryEntry = directoryEntry;
				}
			};
			//------------------------------------------------------------------------


			// Si este elemento se est� arrastrando...
			if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(0, 0.0f))
			{
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					const std::string payload_n = relativePath.string();
					ImGui::SetDragDropPayload("ASSET_DRAG", payload_n.c_str(), payload_n.length() + 1, ImGuiCond_Once);
					ImGui::EndDragDropSource();
				}
			}

			if (thumbnailSize < 40)
			{
				ImGui::SameLine();  // Asegura que el texto aparezca en la misma l�nea que el icono
			}

			ImGui::Text(filenameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}


		if (isDialogOpen) {
			ImGui::OpenPopup("Import Options");
		}


		if (ImGui::BeginPopupModal("Import Options", &isDialogOpen)) 
		{
			// Obtener el fullPath y descomponer en filePath y fileName
			std::filesystem::path fullPath = m_DirectoryEntry;
			std::string filePath = fullPath.parent_path().string();
			std::string fileName = fullPath.filename().string();

			// Mostrar el path y el nombre de archivo
			ImGui::Text("Path: %s", filePath.c_str());
			ImGui::Text("File: %s", fileName.c_str());
			ImGui::Separator();

			// Mostrar las opciones de importaci�n
			ImGui::Checkbox("Invert UV", &importOptions.invertUV);
			ImGui::Checkbox("Rotate 90 degrees on X-axis", &importOptions.rotate90);
			ImGui::SliderFloat("Scale", &importOptions.globalScaleFactor, 0.01f, 10.0f, "%.1f", ImGuiSliderFlags_None);
			ImGui::Separator();

			ImGui::Checkbox("Lights", &importOptions.processLights);

			// Bot�n de Aceptar
			if (ImGui::Button("Accept")) {
				isDialogOpen = false;  // Cierra el di�logo

				if (filePath.back() != '\\' && filePath.back() != '/') {
					filePath += '\\';  // Asegura que el filePath termina en '\'
				}
				importOptions.filePath = filePath;
				importOptions.fileName = fileName;

				AssetsManager::GetInstance().LoadModelAsset(importOptions);

			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				isDialogOpen = false;  // Cierra el di�logo
			}
			ImGui::EndPopup();
		}

		ImGui::Columns(1);

		ImGui::EndChild();
		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 39, 512);
		ImGui::End();
	}
}
