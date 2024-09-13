#pragma once

#include "../LibCoreHeaders.h"
#include "Components.h"
#include <entt.hpp>
#include <memory>
#include <iostream>
#include <sol/sol.hpp>
#include "../Scripting/LuaManager.h"  // Incluir el struct ImportLUA_ScriptData
#include <vector>

namespace libCore
{
    class ScriptComponent {
    public:
        ScriptComponent() = default;
        ScriptComponent(entt::entity entityRef) : entity(entityRef) {}

        // Cargar múltiples scripts Lua usando ImportLUA_ScriptData
        void AddLuaScript(const ImportLUA_ScriptData& scriptData);

        bool HasLuaScripts() const { return !luaScriptsData.empty(); }

        const std::vector<ImportLUA_ScriptData>& GetLuaScriptsData() const { return luaScriptsData; }

        void RemoveLuaScript(const std::string& scriptName);

        // Obtener los valores de exposedVars de un script específico
        std::unordered_map<std::string, sol::object> GetExposedVars(const std::string& scriptName) const;

        // Establecer los valores de exposedVars de un script específico
        void SetExposedVars(const std::string& scriptName, const std::unordered_map<std::string, sol::object>& exposedVars);

        //--LIFE CYCLE
        void Init();
        void Update(float deltaTime);

    private:
        entt::entity entity;
        std::vector<ImportLUA_ScriptData> luaScriptsData;  // Cambiado a lista para múltiples scripts
    };
}
