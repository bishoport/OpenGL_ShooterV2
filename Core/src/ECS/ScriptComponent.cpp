#include "ScriptComponent.h"
#include "EntityManager.h"
#include "../Scripting/LuaManager.h"
#include <glm/gtc/quaternion.hpp>
#include "../tools/AssetsManager.h"

namespace libCore
{
    void ScriptComponent::SetLuaScript(const ImportLUA_ScriptData& scriptData) {
        luaScriptData = scriptData;

        // Verificar si el script ya está cargado antes de pedir a LuaManager que lo cargue
        //if (!LuaManager::GetInstance().IsScriptLoaded(luaScriptData.name)) {
        //    LuaManager::GetInstance().LoadLuaFile(luaScriptData.name, luaScriptData.filePath);
        //}

        // Obtener y almacenar la referencia al estado Lua cargado por LuaManager
        //sol::state& luaState = LuaManager::GetInstance().GetLuaState(luaScriptData.name);
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
}
