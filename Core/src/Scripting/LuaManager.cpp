// LuaManager.cpp
#include "LuaManager.h"
#include "../ECS/EntityManagerBridge.h" // Incluir la clase de EntityManagerBridge

namespace libCore {

    void LuaManager::LoadLuaFile(const std::string& scriptName, const std::string& path)
    {
        auto luaState = std::make_unique<sol::state>();
        luaState->open_libraries(sol::lib::base);

        // Registrar las funciones comunes
        RegisterCommonFunctions(*luaState);

        try {
            luaState->script_file(path);

            // Llenar la estructura con los datos del script
            ImportLUA_ScriptData scriptData;
            scriptData.name = scriptName;
            scriptData.filePath = path;

            // Guardar el script en el mapa
            scripts[scriptName] = std::make_pair(scriptData, std::move(luaState));

            std::cout << "Loaded Lua script: " << scriptName << " from path: " << path << std::endl;
        }
        catch (const sol::error& e) {
            std::cerr << "Error loading Lua script: " << e.what() << std::endl;
        }
    }

    // Funci�n para recargar un script Lua en tiempo de ejecuci�n
    void LuaManager::ReloadLuaFile(const std::string& scriptName)
    {
        auto it = scripts.find(scriptName);
        if (it != scripts.end()) {
            const std::string& path = it->second.first.filePath;

            auto luaState = std::make_unique<sol::state>();
            luaState->open_libraries(sol::lib::base);
            RegisterCommonFunctions(*luaState); // Registrar funciones comunes nuevamente

            try {
                luaState->script_file(path);  // Vuelve a cargar el archivo Lua
                it->second.second = std::move(luaState);  // Reemplaza el estado Lua con el nuevo

                std::cout << "Reloaded Lua script: " << scriptName << " from path: " << path << std::endl;
            }
            catch (const sol::error& e) {
                std::cerr << "Error reloading Lua script: " << e.what() << std::endl;
            }
        }
        else {
            std::cerr << "Script not found to reload: " << scriptName << std::endl;
        }
    }

    void LuaManager::RegisterCommonFunctions(sol::state& luaState) {
        luaState.new_usertype<entt::entity>("entity");

        luaState.new_usertype<EntityManagerBridge>("EntityManagerBridge",
            "CreateEntity", &EntityManagerBridge::CreateEntity,
            "CreateEntityFromModel", &EntityManagerBridge::CreateEntityFromModel,
            "GetEntityName", &EntityManagerBridge::GetEntityName,
            "DestroyEntity", &EntityManagerBridge::DestroyEntity,

            "GetPosition", &EntityManagerBridge::GetPosition,
            "SetPosition", &EntityManagerBridge::SetPosition,
            "GetRotation", &EntityManagerBridge::GetRotation,
            "SetRotation", &EntityManagerBridge::SetRotation,
            "GetScale", &EntityManagerBridge::GetScale,
            "SetScale", &EntityManagerBridge::SetScale
        );

        static EntityManagerBridge entityManagerBridge;
        luaState["EntityManager"] = &entityManagerBridge;
    }

    sol::state& LuaManager::GetLuaState(const std::string& scriptName) const {
        auto it = scripts.find(scriptName);
        if (it != scripts.end()) {
            return *(it->second.second);  // Devuelve la referencia del estado Lua
        }
        throw std::runtime_error("Lua state not found for script: " + scriptName);
    }

    bool LuaManager::IsScriptLoaded(const std::string& scriptName) const {
        return scripts.find(scriptName) != scripts.end();
    }

    std::vector<ImportLUA_ScriptData> LuaManager::GetLoadedScripts() const {
        std::vector<ImportLUA_ScriptData> scriptDataList;
        for (const auto& pair : scripts) {
            scriptDataList.push_back(pair.second.first);
        }
        return scriptDataList;
    }
}
