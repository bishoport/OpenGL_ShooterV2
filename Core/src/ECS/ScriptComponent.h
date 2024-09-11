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
        ScriptComponent(entt::entity entityRef) : entity(entityRef), simpleFloat(0) {}

        void SetLuaScript(const std::string& scriptName);
        bool HasLuaScript() const { return scriptAssigned; }
        const std::string& GetLuaScriptName() const { return luaScriptName; }
        void RemoveLuaScript() { this->luaScriptName.clear(); }

        void RegisterClasses();

        //--LIFE CYCLE
        void Init();
        void Update(float deltaTime);

        //--API
        //--TANSFORM
        std::tuple<float, float, float> GetPosition();
        void SetPosition(float x, float y, float z);

        std::tuple<float, float, float> GetRotation();  // Rotación en Euler
        void SetRotation(float x, float y, float z);    // Establecer rotación en Euler

        std::tuple<float, float, float> GetScale();
        void SetScale(float x, float y, float z);
        //--


    private:
        entt::entity entity;
        std::string luaScriptName;
        bool scriptAssigned = false;

    };
}
