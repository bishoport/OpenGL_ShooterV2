#include "ScriptComponent.h"
#include "EntityManager.h"
#include "../Scripting/LuaManager.h"

namespace libCore
{
    void ScriptComponent::Init()
    {
        if (scriptAssigned) {
            lua_State* L = LuaManager::GetInstance().GetLuaState(luaScriptName);
            if (!L) {
                std::cerr << "Lua state for script " << luaScriptName << " is invalid!" << std::endl;
                return;
            }

            // Registrar el ScriptComponent en Lua como "script"
            LuaManager::GetInstance().RegisterClasses(L, this);

            lua_getglobal(L, "Init");
            if (lua_isfunction(L, -1)) {
                if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                    std::cerr << "Error calling Lua Init: " << lua_tostring(L, -1) << std::endl;
                }
            }
            lua_pop(L, 1);
        }
    }

    void ScriptComponent::Update(float deltaTime)
    {
        if (scriptAssigned) {
            lua_State* L = LuaManager::GetInstance().GetLuaState(luaScriptName);
            if (!L) {
                std::cerr << "Lua state for script " << luaScriptName << " is invalid!" << std::endl;
                return;
            }

            // Registrar el ScriptComponent en Lua como "script"
            LuaManager::GetInstance().RegisterClasses(L, this);

            lua_getglobal(L, "Update");
            if (lua_isfunction(L, -1)) {
                lua_pushnumber(L, deltaTime);
                if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                    std::cerr << "Error calling Lua Update: " << lua_tostring(L, -1) << std::endl;
                }
            }
            lua_pop(L, 1);
        }
    }

    TransformComponent& ScriptComponent::GetTransform()
    {
        if (EntityManager::GetInstance().HasComponent<TransformComponent>(entity)) {
            return EntityManager::GetInstance().GetComponent<TransformComponent>(entity);
        }
        throw std::runtime_error("Entity does not have a TransformComponent.");
    }
 
}
