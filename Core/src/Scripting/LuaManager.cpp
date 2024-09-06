#include "LuaManager.h"
#include "../ECS/Components.h"
#include "../ECS/ScriptComponent.h"

namespace libCore
{
    void LuaManager::LoadLuaFile(const std::string& scriptName, const std::string& path)
    {
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);

        int resultL = luaL_loadfile(L, path.c_str());
        if (resultL == LUA_OK)
        {
            resultL = lua_pcall(L, 0, LUA_MULTRET, 0);
            if (resultL == LUA_OK)
            {
                // Guardar el script en el diccionario
                scripts[scriptName] = std::unique_ptr<lua_State, LuaStateDeleter>(L);
                std::cout << "Loaded Lua script: " << scriptName << " from path: " << path << std::endl;
                printLuaStack(L);
            }
            else
            {
                std::cerr << "Error ejecutando " << path << ": " << lua_tostring(L, -1) << std::endl;
                lua_close(L);
            }
        }
        else
        {
            std::cerr << "Error cargando " << path << ": " << lua_tostring(L, -1) << std::endl;
            lua_close(L);
        }
    }

    lua_State* LuaManager::GetLuaState(const std::string& scriptName) const
    {
        auto it = scripts.find(scriptName);
        if (it != scripts.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    bool LuaManager::IsScriptLoaded(const std::string& scriptName) const
    {
        return scripts.find(scriptName) != scripts.end();
    }

    std::vector<std::string> LuaManager::GetLoadedScripts() const
    {
        std::vector<std::string> scriptNames;
        for (const auto& pair : scripts) {
            scriptNames.push_back(pair.first);
        }
        return scriptNames;
    }

    void LuaManager::printLuaStack(lua_State* L) {
        int top = lua_gettop(L);  // Obtiene el número de elementos en la pila
        printf("Total in stack: %d\n", top);

        for (int i = 1; i <= top; i++) {
            int t = lua_type(L, i);
            switch (t) {
            case LUA_TSTRING:  // Si el elemento es una cadena
                printf("string: '%s'\n", lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:  // Si el elemento es un booleano
                printf("boolean: %s\n", lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:  // Si el elemento es un número
                printf("number: %g\n", lua_tonumber(L, i));
                break;
            default:  // Para otros tipos de datos
                printf("%s\n", lua_typename(L, t));
                break;
            }
        }
    }

    void LuaManager::RegisterClasses(lua_State* L, ScriptComponent* scriptComponent)
    {
        // Crear una tabla Lua para "script" que contenga las funciones de ScriptComponent
        lua_newtable(L);

        // getTransform function
        lua_pushlightuserdata(L, scriptComponent);  // Pasar el puntero al ScriptComponent
        lua_pushcclosure(L, [](lua_State* L) -> int {
            ScriptComponent* script = static_cast<ScriptComponent*>(lua_touserdata(L, lua_upvalueindex(1)));
            if (script) {
                try {
                    TransformComponent& transform = script->GetTransform();
                    lua_pushlightuserdata(L, &transform);  // Empuja el puntero del transform a la pila
                    return 1;
                }
                catch (const std::exception& e) {
                    lua_pushnil(L);
                    lua_pushstring(L, e.what());
                    return 2;
                }
            }
            lua_pushnil(L);
            return 1;
            }, 1);
        lua_setfield(L, -2, "getTransform");

        // getPosition function
        lua_pushlightuserdata(L, scriptComponent);  // Pasar el puntero al ScriptComponent
        lua_pushcclosure(L, [](lua_State* L) -> int {
            ScriptComponent* script = static_cast<ScriptComponent*>(lua_touserdata(L, lua_upvalueindex(1)));
            if (script) {
                TransformComponent& transform = script->GetTransform();
                glm::vec3 position = transform.transform->GetPosition();

                // Crear una tabla Lua para la posición
                lua_newtable(L);
                lua_pushnumber(L, position.x);
                lua_setfield(L, -2, "x");
                lua_pushnumber(L, position.y);
                lua_setfield(L, -2, "y");
                lua_pushnumber(L, position.z);
                lua_setfield(L, -2, "z");
                return 1;  // Retorna la tabla con la posición
            }
            lua_pushnil(L);
            return 1;
            }, 1);
        lua_setfield(L, -2, "getPosition");

        // setPosition function
        lua_pushlightuserdata(L, scriptComponent);  // Pasar el puntero al ScriptComponent
        lua_pushcclosure(L, [](lua_State* L) -> int {
            ScriptComponent* script = static_cast<ScriptComponent*>(lua_touserdata(L, lua_upvalueindex(1)));
            if (script) {
                TransformComponent& transform = script->GetTransform();

                // Obtener la posición desde Lua (x, y, z en una tabla)
                lua_getfield(L, -1, "x");
                lua_getfield(L, -2, "y");
                lua_getfield(L, -3, "z");

                glm::vec3 position(
                    static_cast<float>(lua_tonumber(L, -3)),
                    static_cast<float>(lua_tonumber(L, -2)),
                    static_cast<float>(lua_tonumber(L, -1))
                );
                transform.transform->SetPosition(position);
            }
            return 0;
            }, 1);
        lua_setfield(L, -2, "setPosition");

        // Asignar la tabla globalmente como "script"
        lua_setglobal(L, "script");
    }

}
