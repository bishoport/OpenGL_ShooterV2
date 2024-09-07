#pragma once

#include "../LibCoreHeaders.h"
#include "Components.h"
#include <entt.hpp>
#include <memory>
#include <iostream>
#include <sol/sol.hpp>

namespace libCore
{
    class ScriptComponent {
    public:
        ScriptComponent() = default;
        ScriptComponent(entt::entity entityRef) : entity(entityRef), simpleInt(0) {}

        void SetLuaScript(const std::string& scriptName);
        bool HasLuaScript() const { return scriptAssigned; }
        const std::string& GetLuaScriptName() const { return luaScriptName; }
        void RemoveLuaScript() { this->luaScriptName.clear(); }

        void RegisterClasses();

        //--LIFE CYCLE
        void Init();
        void Update(float deltaTime);

        //--METHODS
        TransformComponent& GetTransform();

        int GetSimpleInt() const;
        void SetSimpleInt(int value);

    private:
        entt::entity entity;
        std::string luaScriptName;
        bool scriptAssigned = false;
        int simpleInt = 25;  // Variable de prueba para exponer a Lua
    };
}
