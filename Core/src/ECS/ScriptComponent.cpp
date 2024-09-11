#include "ScriptComponent.h"
#include "EntityManager.h"
#include "../Scripting/LuaManager.h"

namespace libCore
{
    //--INICIALIZACION DEL COMPONENTE
    void ScriptComponent::SetLuaScript(const std::string& scriptName) {
        luaScriptName = scriptName;

        // Obtener el estado Lua de LuaManager
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);

        // Registrar las clases y componentes C++ que se pueden usar desde Lua
        RegisterClasses();
        scriptAssigned = true;
    }
    void ScriptComponent::RegisterClasses() {
        sol::state& L = LuaManager::GetInstance().GetLuaState(luaScriptName);

        // Exponer ScriptComponent a Lua
        L.new_usertype<ScriptComponent>("ScriptComponent",
            "GetPosition", &ScriptComponent::GetPosition,
            "SetPosition", &ScriptComponent::SetPosition
        );

        // Exponer el ScriptComponent global a Lua para su uso en scripts
        L["script"] = this;
    }

    //-------------------------------------------------------------------------------------------------------------------------

    //--API
    std::tuple<float, float, float> ScriptComponent::GetPosition() 
    {
        if (EntityManager::GetInstance().HasComponent<TransformComponent>(entity)) {
            glm::vec3 position = EntityManager::GetInstance().GetComponent<TransformComponent>(entity).GetPosition();
            //std::cout << "Position from TransformComponent: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;

            // Devolvemos los valores x, y, z como un std::tuple para que Lua los reciba correctamente
            return std::make_tuple(position.x, position.y, position.z);
        }
        else {
            std::cerr << "TransformComponent not found for entity." << std::endl;
            // Devuelve una posición por defecto si no existe el componente
            return std::make_tuple(0.0f, 0.0f, 0.0f);
        }
    }

    // Función para establecer la posición usando tres floats
    void ScriptComponent::SetPosition(float x, float y, float z) 
    {
        if (EntityManager::GetInstance().HasComponent<TransformComponent>(entity)) {
            EntityManager::GetInstance().GetComponent<TransformComponent>(entity).SetPosition(glm::vec3(x, y, z));
        }
        else 
        {
            std::cerr << "TransformComponent not found for entity." << std::endl;
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------

    //--LIFE CYCLE
    void ScriptComponent::Init() {
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);
        sol::function initFunc = lua["Init"];
        if (initFunc.valid()) {
            initFunc();
        }
        else {
            std::cerr << "Lua Init function not found" << std::endl;
        }
    }
    void ScriptComponent::Update(float deltaTime)
    {
        sol::state& lua = LuaManager::GetInstance().GetLuaState(luaScriptName);
        sol::function updateFunc = lua["Update"];
        if (updateFunc.valid())
        {
            updateFunc(deltaTime);
        }
        else {
            std::cerr << "Lua Update function not found" << std::endl;
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------

    //--TEST FUNCTIONS
    float ScriptComponent::GetSimpleFloat() const {
        return simpleFloat;
    }

    void ScriptComponent::SetSimpleFloat(float value) {
        simpleFloat = value;

        if (EntityManager::GetInstance().HasComponent<TransformComponent>(entity)) {
            EntityManager::GetInstance().GetComponent<TransformComponent>(entity).SetPosition(glm::vec3(0.0f, simpleFloat, 0.0f));
        }
    }
}
