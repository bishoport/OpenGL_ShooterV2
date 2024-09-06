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

        // Constructor que recibe solo la referencia de la entidad
        ScriptComponent(entt::entity entityRef) : entity(entityRef) {}

        // Función para asociar un script Lua de la biblioteca
        void SetLuaScript(const std::string& scriptName) {
            luaScriptName = scriptName;
            scriptAssigned = true;
        }

        bool HasLuaScript() const {
            return scriptAssigned;
        }

        const std::string& GetLuaScriptName() const {
            return luaScriptName;
        }

        void RemoveLuaScript() {
            this->luaScriptName.clear();  // Limpia el nombre del script asignado
        }

        // Función para inicializar el script Lua
        void Init();

        // Función para actualizar el script Lua
        void Update(float deltaTime);



        // Funciones para acceder a otros componentes de la entidad desde Lua
        TransformComponent& GetTransform();

        CameraComponent& GetCamera();
    };
}