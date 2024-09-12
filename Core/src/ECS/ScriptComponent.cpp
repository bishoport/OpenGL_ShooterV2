#include "ScriptComponent.h"
#include "EntityManager.h"
#include "../Scripting/LuaManager.h"
#include <glm/gtc/quaternion.hpp>
#include "../tools/AssetsManager.h"

namespace libCore
{
    void ScriptComponent::SetLuaScript(const std::string& scriptName) {
        luaScriptName = scriptName;

        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);

        RegisterClasses();
        scriptAssigned = true;
    }

    void ScriptComponent::RegisterClasses() {
        sol::state& L = LuaManager::GetInstance().GetLuaState(luaScriptName);

        // Exponer ScriptComponent a Lua
        L.new_usertype<ScriptComponent>("ScriptComponent",
            "GetPosition",   &ScriptComponent::GetPosition,
            "SetPosition",   &ScriptComponent::SetPosition,
            "GetRotation",   &ScriptComponent::GetRotation,
            "SetRotation",   &ScriptComponent::SetRotation,
            "GetScale",      &ScriptComponent::GetScale,
            "SetScale",      &ScriptComponent::SetScale
        );

        // Exponer el ScriptComponent global a Lua para su uso en scripts
        L["script"] = this;
    }

    //-------------------------------------------------------------------------------------------------------------------------

    //--API
    std::tuple<float, float, float> ScriptComponent::GetPosition() {
        if (EntityManager::GetInstance().HasComponent<TransformComponent>(entity)) {
            glm::vec3 position = EntityManager::GetInstance().GetComponent<TransformComponent>(entity).GetPosition();
            return std::make_tuple(position.x, position.y, position.z);
        }
        return std::make_tuple(0.0f, 0.0f, 0.0f);
    }

    void ScriptComponent::SetPosition(float x, float y, float z) {
        if (EntityManager::GetInstance().HasComponent<TransformComponent>(entity)) {
            EntityManager::GetInstance().GetComponent<TransformComponent>(entity).SetPosition(glm::vec3(x, y, z));
        }
    }

    std::tuple<float, float, float> ScriptComponent::GetRotation() {
        if (EntityManager::GetInstance().HasComponent<TransformComponent>(entity)) {
            glm::vec3 eulerAngles = EntityManager::GetInstance().GetComponent<TransformComponent>(entity).GetEulerAngles();
            return std::make_tuple(eulerAngles.x, eulerAngles.y, eulerAngles.z);
        }
        return std::make_tuple(0.0f, 0.0f, 0.0f);
    }

    void ScriptComponent::SetRotation(float x, float y, float z) {
        if (EntityManager::GetInstance().HasComponent<TransformComponent>(entity)) {
            EntityManager::GetInstance().GetComponent<TransformComponent>(entity).SetRotationEuler(glm::vec3(x, y, z));
        }
    }

    std::tuple<float, float, float> ScriptComponent::GetScale() {
        if (EntityManager::GetInstance().HasComponent<TransformComponent>(entity)) {
            glm::vec3 scale = EntityManager::GetInstance().GetComponent<TransformComponent>(entity).GetScale();
            return std::make_tuple(scale.x, scale.y, scale.z);
        }
        return std::make_tuple(1.0f, 1.0f, 1.0f);  // Devolver escala por defecto si no tiene componente
    }

    void ScriptComponent::SetScale(float x, float y, float z) {
        if (EntityManager::GetInstance().HasComponent<TransformComponent>(entity)) {
            EntityManager::GetInstance().GetComponent<TransformComponent>(entity).SetScale(glm::vec3(x, y, z));
        }
    }

    //void ScriptComponent::InstanceModel(std::string key)
    //{
    //    EntityManager::GetInstance().CreateGameObjectFromModel( AssetsManager::GetInstance().GetModel(key), entt::null);
    //}

    //-------------------------------------------------------------------------------------------------------------------------

    void ScriptComponent::Init() {
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);
        sol::function initFunc = lua["Init"];
        if (initFunc.valid()) {
            initFunc();
        }
    }

    void ScriptComponent::Update(float deltaTime) {
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);
        sol::function updateFunc = lua["Update"];
        if (updateFunc.valid()) {
            updateFunc(deltaTime);
        }
        else {
            std::cerr << "Lua Update function not found" << std::endl;
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
}
