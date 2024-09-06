#pragma once
#include "../LibCoreHeaders.h"
#include <lua/lua.hpp>
#include <LuaBridge/LuaBridge.h>

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

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
        lua_State* GetLuaState(const std::string& scriptName) const
        {
            auto it = scripts.find(scriptName);
            if (it != scripts.end()) {
                if (!it->second) {
                    std::cerr << "Lua state for script " << scriptName << " is null!" << std::endl;
                }
                return it->second.get();
            }
            std::cerr << "Lua state not found for script: " << scriptName << std::endl;
            return nullptr; // Retorna nullptr si no se encuentra
        }

        // Función para verificar si un script está cargado
        bool IsScriptLoaded(const std::string& scriptName) const {
            return scripts.find(scriptName) != scripts.end();
        }

        // Método para obtener una lista de los nombres de los scripts cargados
        std::vector<std::string> GetLoadedScripts() const 
        {
            std::vector<std::string> scriptNames;
            for (const auto& pair : scripts) {
                scriptNames.push_back(pair.first);
            }
            return scriptNames;
        }

        // Función para registrar clases y funciones de C++ en Lua
        void RegisterClasses(lua_State* L);

        void printLuaStack(lua_State* L);

    private:
        LuaManager() {
            // Constructor privado
        }

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

