#pragma once

#include "../LibCoreHeaders.h"
#include "Components.h"
#include <entt.hpp>
#include <memory>
#include <iostream>

namespace libCore
{
    class ScriptComponent {
    public:

        entt::entity entity;
        std::string luaScriptName;
        bool scriptAssigned = false;

        ScriptComponent() = default;
        ScriptComponent(entt::entity entityRef) : entity(entityRef) {}

        void SetLuaScript(const std::string& scriptName) {
            luaScriptName = scriptName;
            scriptAssigned = true;
        }

        bool HasLuaScript() const { return scriptAssigned; }
        const std::string& GetLuaScriptName() const { return luaScriptName; }
        void RemoveLuaScript() { this->luaScriptName.clear(); }

        void Init();
        void Update(float deltaTime);

        TransformComponent& GetTransform();
        CameraComponent& GetCamera();
    };
}
