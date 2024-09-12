#include "LuaManager.h"
#include "../ECS/EntityManagerBridge.h" // Incluir la clase de EntityManagerBridge

namespace libCore {

    void LuaManager::LoadLuaFile(const std::string& scriptName, const std::string& path) {
        auto luaState = std::make_unique<sol::state>();
        luaState->open_libraries(sol::lib::base);

        // Registrar las funciones comunes
        RegisterCommonFunctions(*luaState);

        try {
            luaState->script_file(path);
            scripts[scriptName] = std::move(luaState);
            std::cout << "Loaded Lua script: " << scriptName << " from path: " << path << std::endl;
        }
        catch (const sol::error& e) {
            std::cerr << "Error loading Lua script: " << e.what() << std::endl;
        }
    }

    void LuaManager::RegisterCommonFunctions(sol::state& luaState) {
        // Exponer `entt::entity` como un tipo simple en Lua
        luaState.new_usertype<entt::entity>("entity");

        // Exponer `EntityManagerBridge` a Lua
        luaState.new_usertype<EntityManagerBridge>("EntityManagerBridge",
            "CreateEntity", &EntityManagerBridge::CreateEntity,
            "CreateEntityFromModel", &EntityManagerBridge::CreateEntityFromModel,
            "GetEntityName", &EntityManagerBridge::GetEntityName,
            "DestroyEntity", &EntityManagerBridge::DestroyEntity,

            // Métodos de TransformComponent
            "GetPosition", &EntityManagerBridge::GetPosition,
            "SetPosition", &EntityManagerBridge::SetPosition,
            "GetRotation", &EntityManagerBridge::GetRotation,
            "SetRotation", &EntityManagerBridge::SetRotation,
            "GetScale", &EntityManagerBridge::GetScale,
            "SetScale", &EntityManagerBridge::SetScale
        );

        // Crear una instancia global de `EntityManagerBridge`
        static EntityManagerBridge entityManagerBridge;
        luaState["EntityManager"] = &entityManagerBridge;
    }


    sol::state& LuaManager::GetLuaState(const std::string& scriptName) const {
        auto it = scripts.find(scriptName);
        if (it != scripts.end()) {
            return *(it->second);  // Devuelve la referencia del estado Lua
        }
        throw std::runtime_error("Lua state not found for script: " + scriptName);
    }

    bool LuaManager::IsScriptLoaded(const std::string& scriptName) const {
        return scripts.find(scriptName) != scripts.end();
    }

    std::vector<std::string> LuaManager::GetLoadedScripts() const {
        std::vector<std::string> scriptNames;
        for (const auto& pair : scripts) {
            scriptNames.push_back(pair.first);
        }
        return scriptNames;
    }
}
