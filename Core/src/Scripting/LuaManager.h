#pragma once

#include "../LibCoreHeaders.h"
#include <sol/sol.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "../ECS/EntityManagerBridge.h"

namespace libCore {

    class LuaManager {

    public:
        // Singleton
        static LuaManager& GetInstance() {
            static LuaManager instance;
            return instance;
        }

        // Cargar un archivo Lua y almacenar su estado
        void LoadLuaFile(const std::string& scriptName, const std::string& path);

        // Recargar un archivo Lua
        void ReloadLuaFile(const std::string& scriptName);

        // Obtener el estado Lua por nombre de script
        sol::state& GetLuaState(const std::string& scriptName) const;

        // Registrar las funciones comunes (como EntityManagerBridge)
        void RegisterCommonFunctions(sol::state& luaState);

        // Verificar si un script est� cargado
        bool IsScriptLoaded(const std::string& scriptName) const;

        // Obtener lista de scripts cargados
        std::vector<ImportLUA_ScriptData> GetLoadedScripts() const;

    private:
        LuaManager() {}
        ~LuaManager() {}

        // Diccionario para almacenar los estados de los scripts Lua junto con su informaci�n
        std::unordered_map<std::string, std::pair<ImportLUA_ScriptData, std::unique_ptr<sol::state>>> scripts;
    };
}
