#include "ScriptComponent.h"
#include "EntityManager.h"
#include "../Scripting/LuaManager.h"
#include <glm/gtc/quaternion.hpp>
#include "../tools/AssetsManager.h"

namespace libCore
{
    void ScriptComponent::SetLuaScript(const std::string& scriptName) {
        luaScriptName = scriptName;

        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);

        //RegisterClasses();
        scriptAssigned = true;
    }

    void ScriptComponent::RegisterClasses() {
        //sol::state& L = LuaManager::GetInstance().GetLuaState(luaScriptName);

        //// Exponer ScriptComponent a Lua
        //L.new_usertype<ScriptComponent>("ScriptComponent",
        //    "GetPosition",   &ScriptComponent::GetPosition,
        //    "SetPosition",   &ScriptComponent::SetPosition,
        //    "GetRotation",   &ScriptComponent::GetRotation,
        //    "SetRotation",   &ScriptComponent::SetRotation,
        //    "GetScale",      &ScriptComponent::GetScale,
        //    "SetScale",      &ScriptComponent::SetScale
        //);

        //// Exponer el ScriptComponent global a Lua para su uso en scripts
        //L["script"] = this;
    }
    //-------------------------------------------------------------------------------------------------------------------------


    //--LIFE CYCLE
    void ScriptComponent::Init() {
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);
        sol::function initFunc = lua["Init"];
        if (initFunc.valid()) {
            initFunc();
        }
    }
    void ScriptComponent::Update(float deltaTime) {
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);
        sol::function updateFunc = lua["Update"];
        if (updateFunc.valid()) {
            updateFunc(deltaTime);
        }
        else {
            std::cerr << "Lua Update function not found" << std::endl;
        }
    }
    //-------------------------------------------------------------------------------------------------------------------------
}
