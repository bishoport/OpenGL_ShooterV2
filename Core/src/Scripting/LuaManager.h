#pragma once
#include "../LibCoreHeaders.h"
#include <sol/sol.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "../ECS/EntityManagerBridge.h" // Incluir el bridge

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

        // Obtener el estado Lua por nombre de script
        sol::state& GetLuaState(const std::string& scriptName) const;

        // Registrar las funciones comunes (como EntityManagerBridge)
        void RegisterCommonFunctions(sol::state& luaState);

        // Verificar si un script está cargado
        bool IsScriptLoaded(const std::string& scriptName) const;

        // Obtener lista de scripts cargados
        std::vector<std::string> GetLoadedScripts() const;

    private:
        LuaManager() {}
        ~LuaManager() {}

        // Diccionario para almacenar los estados de los scripts Lua
        std::unordered_map<std::string, std::unique_ptr<sol::state>> scripts;
    };
}
