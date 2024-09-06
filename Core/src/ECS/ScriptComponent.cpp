#include "ScriptComponent.h"
#include "EntityManager.h"
#include "../Scripting/LuaManager.h"

namespace libCore
{
    void ScriptComponent::Init()
    {
        if (scriptAssigned) {
            lua_State* L = LuaManager::GetInstance().GetLuaState(luaScriptName);

            // Pasar el ScriptComponent a Lua como la variable global "script"
            luabridge::push(L, this);
            lua_setglobal(L, "script");

            lua_getglobal(L, "Init");

            LuaManager::GetInstance().printLuaStack(L);

            if (lua_isfunction(L, -1)) {
                //std::cout << "Calling Lua Init function" << std::endl;
                if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                    std::cerr << "Error calling Lua Init: " << lua_tostring(L, -1) << std::endl;
                }
                LuaManager::GetInstance().printLuaStack(L);
            }
            else {
                std::cerr << "Lua Init function not found" << std::endl;
            }
            lua_pop(L, 1);
        }
    }

    void ScriptComponent::Update(float deltaTime)
    {
        if (!scriptAssigned || luaScriptName.empty()) {
            std::cerr << "Lua script not assigned or invalid!" << std::endl;
            return;
        }

        lua_State* L = LuaManager::GetInstance().GetLuaState(luaScriptName);
        if (!L) {
            std::cerr << "Lua state for script " << luaScriptName << " is invalid!" << std::endl;
            return;
        }

        // Pasar el ScriptComponent a Lua como la variable global "script"
        luabridge::push(L, this);
        lua_setglobal(L, "script");

        lua_getglobal(L, "Update");

        if (lua_isfunction(L, -1)) {
            lua_pushnumber(L, deltaTime);
            if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                std::cerr << "Error calling Lua Update: " << lua_tostring(L, -1) << std::endl;
            }
        }
        else {
            std::cerr << "Lua Update function not found" << std::endl;
        }
        lua_pop(L, 1);
    }


    TransformComponent& ScriptComponent::GetTransform()
    {
        // Verificar que la entidad es válida
        if (EntityManager::GetInstance().HasComponent<TransformComponent>(entity)) 
        {
            return EntityManager::GetInstance().GetComponent<TransformComponent>(entity);
        }
        throw std::runtime_error("Entity does not have a TransformComponent.");
    }

    CameraComponent& ScriptComponent::GetCamera()
    {
        if (EntityManager::GetInstance().HasComponent<CameraComponent>(entity)) 
        {
            return EntityManager::GetInstance().GetComponent<CameraComponent>(entity);
        }
        throw std::runtime_error("Entity does not have a CameraComponent.");
    }
}
