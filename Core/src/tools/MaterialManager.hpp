#pragma once
#include "../LibCoreHeaders.h"
#include "../Core/Material.h"
#include <mutex>
#include <imgui.h>
#include "AssetsManager.h"

namespace libCore
{
    class MaterialManager
    {

    public:
        static MaterialManager& getInstance() {
            static MaterialManager instance;
            return instance;
        }

        // Elimina los métodos de copia y asignación
        MaterialManager(MaterialManager const&) = delete;
        void operator=(MaterialManager const&) = delete;


        void CreateEmptyMaterial(const std::string& matName)
        {
            Ref <Material> material = std::make_shared<Material>(matName);
            material->materialName = matName;
            addMaterial(material);
        }

        // Agrega un nuevo material al manager
        Ref<Material> addMaterial( Ref<Material> materialData) {
            auto result = materials.emplace(materialData->materialName, materialData);

            if (result.second) {
                // El material fue insertado correctamente.
                return materialData;
            }
            else {
                // El material ya existía, devuelve el material existente.
                return result.first->second;
            }
        }

        // Obtiene un material por su clave
        Ref<Material> getMaterial(const std::string& key) {
            auto it = materials.find(key);
            if (it != materials.end()) {
                return it->second;
            }
            std::cout << key << " material does exist" << std::endl;
            return nullptr; // O manejar el error como prefieras
        }

        // Elimina un material por su clave
        bool removeMaterial(const std::string& key) {
            return materials.erase(key) > 0;
        }




        std::unordered_map<std::string, Ref<Material>> materials;
        static char materialNameBuffer[128];
    private:
        MaterialManager() {} // Constructor privado
        
        
    };
}



//DRAW IMGUI


//void HandleDragDropForTexture(Ref<Texture> texture, const char* payloadType) {
//    // Crear un área de drop. Si algo está siendo arrastrado sobre este área (que cumple con payloadType), resáltalo
//    if (ImGui::BeginDragDropTarget()) {
//        // Aceptar una carga útil de tipo payloadType. La carga útil necesita contener el identificador de la textura.
//        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType);
//        if (payload != nullptr && payload->IsDataType(payloadType)) {

//            if (!ImGui::IsMouseDown(0))  // Chequeo si el botón izquierdo del ratón se ha liberado
//            {
//                std::string dropped_fpath = (const char*)payload->Data;

//                texture->image = GLCore::Utils::ImageLoader::loadImage("assets/" + dropped_fpath);
//                texture->Bind();
//            }
//        }
//        ImGui::EndDragDropTarget();
//    }
//}

//void showTexture(const char* label, Ref<Texture> texture)
//{
//    // Muestra la textura
//    ImGui::Image((void*)(intptr_t)texture->textureID, ImVec2(128, 128)); // Ajustar según el tamaño deseado
//    HandleDragDropForTexture(texture, "ASSET_DRAG");
//    if (texture && texture->hasMap) {

//        ImGui::Text("%s", label); // Muestra el nombre de la textura
//        ImGui::Text("Path: %s", texture->image->path.c_str()); // Muestra la ruta de la textura
//    }
//    else {
//        ImGui::Text("%s: None", label); // Para texturas que no existen
//    }
//}