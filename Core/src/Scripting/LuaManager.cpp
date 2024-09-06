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

                // Asegúrate de registrar las clases después de cargar el archivo Lua
                RegisterClasses(L);

                std::cout << "Loaded Lua script: " << scriptName << " from path: " << path << std::endl;
                printLuaStack(L);
            }
            else
            {
                std::cerr << "Error ejecutando " << path << ": " << lua_tostring(L, -1) << std::endl;
                lua_close(L); // Cerrar el estado de Lua
            }
        }
        else
        {
            std::cerr << "Error cargando " << path << ": " << lua_tostring(L, -1) << std::endl;
            lua_close(L); // Cerrar el estado de Lua
        }
    }


    void LuaManager::RegisterClasses(lua_State* L)
    {
        // Registrar glm::vec3
        luabridge::getGlobalNamespace(L)
            .beginClass<glm::vec3>("vec3")
            .addConstructor<void(*)()>()
            .addData("x", &glm::vec3::x)
            .addData("y", &glm::vec3::y)
            .addData("z", &glm::vec3::z)
            .endClass()

            // Registrar TransformComponent
            .beginClass<TransformComponent>("TransformComponent")
            .addFunction("getPosition", &TransformComponent::SafeGetPosition)
            .addFunction("setPosition", &TransformComponent::SafeSetPosition)
            .addFunction("getRotation", &TransformComponent::SafeGetRotation)
            .addFunction("setRotation", &TransformComponent::SafeSetRotation)
            .addFunction("getScale", &TransformComponent::SafeGetScale)
            .addFunction("setScale", &TransformComponent::SafeSetScale)
            .endClass();
    }




    void LuaManager::printLuaStack(lua_State* L) {
        int top = lua_gettop(L);
        printf("Total in stack: %d\n", top);
        for (int i = 1; i <= top; i++) {
            int t = lua_type(L, i);
            switch (t) {
            case LUA_TSTRING:  // strings
                printf("string: '%s'\n", lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:  // booleans
                printf("boolean %s\n", lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:  // numbers
                printf("number: %g\n", lua_tonumber(L, i));
                break;
            default:  // other values
                printf("%s\n", lua_typename(L, t));
                break;
            }
        }
    }


}
