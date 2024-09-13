#pragma once

#include "../LibCoreHeaders.h"
#include "Components.h"
#include <entt.hpp>
#include <memory>
#include <iostream>
#include <sol/sol.hpp>
#include "../Scripting/LuaManager.h"  // Incluir el struct ImportLUA_ScriptData

namespace libCore
{
    class ScriptComponent {
    public:
        ScriptComponent() = default;
        ScriptComponent(entt::entity entityRef) : entity(entityRef) {}

        // Cargar el script Lua usando ImportLUA_ScriptData
        void SetLuaScript(const ImportLUA_ScriptData& scriptData);

        bool HasLuaScript() const { return scriptAssigned; }
        const std::string& GetLuaScriptName() const { return luaScriptData.name; }
        const std::string& GetLuaScriptPath() const { return luaScriptData.filePath; }
        const ImportLUA_ScriptData& GetLuaScriptData() const { return luaScriptData; }  // Getter para el struct completo
        void RemoveLuaScript() { this->luaScriptData.name.clear(); }

        // Obtener los valores de exposedVars
        std::unordered_map<std::string, sol::object> GetExposedVars() const;

        // Establecer los valores de exposedVars
        void SetExposedVars(const std::unordered_map<std::string, sol::object>& exposedVars);

        //--LIFE CYCLE
        void Init();
        void Update(float deltaTime);

    private:
        entt::entity entity;
        ImportLUA_ScriptData luaScriptData;  // Cambiado a ImportLUA_ScriptData para más control
        bool scriptAssigned = false;
    };
}
