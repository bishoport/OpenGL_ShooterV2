#include "ScriptComponent.h"
#include "EntityManager.h"
#include "../Scripting/LuaManager.h"
#include <glm/gtc/quaternion.hpp>
#include "../tools/AssetsManager.h"

namespace libCore
{
    void ScriptComponent::SetLuaScript(const ImportLUA_ScriptData& scriptData) {
        luaScriptData = scriptData;
        scriptAssigned = true;
    }
    //-------------------------------------------------------------------------------------------------------------------------


    //--LIFE CYCLE
    void ScriptComponent::Init() {
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptData.name);
        sol::function initFunc = lua["Init"];
        if (initFunc.valid()) {
            initFunc();
        }
    }
    void ScriptComponent::Update(float deltaTime) {
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptData.name);
        sol::function updateFunc = lua["Update"];
        if (updateFunc.valid()) {
            updateFunc(deltaTime);
        }
        else {
            std::cerr << "Lua Update function not found" << std::endl;
        }
    }
    //-------------------------------------------------------------------------------------------------------------------------

    // Obtener los valores de exposedVars
    std::unordered_map<std::string, sol::object> ScriptComponent::GetExposedVars() const {
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptData.name);
        sol::table exposedVars = lua["exposedVars"];

        std::unordered_map<std::string, sol::object> vars;

        if (exposedVars.valid()) {
            for (auto& pair : exposedVars) {
                std::string varName = pair.first.as<std::string>();
                sol::object varValue = pair.second;
                vars[varName] = varValue;  // Guardar los valores actuales
            }
        }
        return vars;
    }

    void ScriptComponent::SetExposedVars(const std::unordered_map<std::string, sol::object>& vars) {
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptData.name);
        sol::table exposedVars = lua["exposedVars"];

        if (exposedVars.valid()) {
            for (const auto& [varName, varValue] : vars) {
                // Asegurarse de que el valor correcto se asigna a la tabla
                if (varValue.is<float>()) {
                    exposedVars[varName] = varValue.as<float>();
                }
                else if (varValue.is<int>()) {
                    exposedVars[varName] = varValue.as<int>();
                }
                else if (varValue.is<bool>()) {
                    exposedVars[varName] = varValue.as<bool>();
                }
                else if (varValue.is<std::string>()) {
                    exposedVars[varName] = varValue.as<std::string>();
                }
            }
        }
    }

}
