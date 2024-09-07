#include "ScriptComponent.h"
#include "EntityManager.h"
#include "../Scripting/LuaManager.h"

namespace libCore
{
    void ScriptComponent::SetLuaScript(const std::string& scriptName) {
        luaScriptName = scriptName;

        // Obtener el estado Lua de LuaManager
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);

        // Registrar las clases y componentes C++ que se pueden usar desde Lua
        RegisterClasses();
        scriptAssigned = true;
    }

    void ScriptComponent::RegisterClasses() {
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);

        // Exponer ScriptComponent a Lua
        lua.new_usertype<ScriptComponent>("ScriptComponent",
            "getSimpleInt", &ScriptComponent::GetSimpleInt,
            "setSimpleInt", &ScriptComponent::SetSimpleInt,
            "getTransform", &ScriptComponent::GetTransform
        );

        // Exponer otros componentes como TransformComponent
        lua.new_usertype<TransformComponent>("TransformComponent",
            "getPosition", &TransformComponent::GetPosition,
            "setPosition", &TransformComponent::SetPosition
        );

        // Exponer el ScriptComponent global a Lua para su uso en scripts
        lua["script"] = this;
    }

    void ScriptComponent::Init() {
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);
        sol::function initFunc = lua["Init"];
        if (initFunc.valid()) {
            initFunc();
        }
    }

    void ScriptComponent::Update(float deltaTime) {
        std::cout << "Calling Lua Update function" << std::endl;

        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);
        sol::function updateFunc = lua["Update"];
        if (updateFunc.valid()) {
            updateFunc(deltaTime);
            std::cout << "Lua Update called successfully" << std::endl;
        }
        else {
            std::cerr << "Lua Update function not found" << std::endl;
        }

        std::cout << "Update function finished" << std::endl;
    }

    TransformComponent& ScriptComponent::GetTransform() {
        if (EntityManager::GetInstance().HasComponent<TransformComponent>(entity)) {
            return EntityManager::GetInstance().GetComponent<TransformComponent>(entity);
        }
        throw std::runtime_error("Entity does not have a TransformComponent.");
    }

    int ScriptComponent::GetSimpleInt() const {
        std::cout << "Getting simpleInt: " << simpleInt << std::endl;
        return simpleInt;
    }

    void ScriptComponent::SetSimpleInt(int value) {
        std::cout << "Setting simpleInt to: " << value << std::endl;
        simpleInt = value;
    }
}
