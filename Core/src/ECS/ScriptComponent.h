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
        std::tuple<float, float, float> GetPosition();   // Para obtener la posición del objeto
        void SetPosition(float x, float y, float z);  // Para establecer la posición del objeto

        //--TEST FUNCTIONS
        float GetSimpleFloat() const;
        void SetSimpleFloat(float value);

    private:
        entt::entity entity;
        std::string luaScriptName;
        bool scriptAssigned = false;

        // Variable de prueba para exponer a Lua
        float simpleFloat = 25.0f;
    };
}
