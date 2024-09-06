#pragma once
#include "../LibCoreHeaders.h"
#include <lua/lua.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "../ECS/ScriptComponent.h"

namespace libCore
{
    class LuaManager
    {
    public:
        // Método para obtener la instancia de la clase
        static LuaManager& GetInstance() {
            static LuaManager instance;
            return instance;
        }

        // Cargar un archivo Lua y almacenarlo en la biblioteca
        void LoadLuaFile(const std::string& scriptName, const std::string& path);

        // Obtener un estado Lua por nombre de script
        lua_State* GetLuaState(const std::string& scriptName) const;

        // Función para verificar si un script está cargado
        bool IsScriptLoaded(const std::string& scriptName) const;

        // Método para obtener una lista de los nombres de los scripts cargados
        std::vector<std::string> GetLoadedScripts() const;

        // Función para registrar clases y funciones de C++ en Lua
        void RegisterClasses(lua_State* L, ScriptComponent* scriptComponent);

        void printLuaStack(lua_State* L);

    private:
        LuaManager() {}
        ~LuaManager() {}

        // Definición personalizada para liberar los lua_State en el map
        struct LuaStateDeleter {
            void operator()(lua_State* L) const {
                if (L) lua_close(L);
            }
        };

        // Diccionario para almacenar los scripts Lua cargados
        std::unordered_map<std::string, std::unique_ptr<lua_State, LuaStateDeleter>> scripts;
    };
}
