#pragma once


#ifdef MY_DLL_EXPORTS
#define MY_DLL_API __declspec(dllexport)
#else
#define MY_DLL_API __declspec(dllimport)
#endif


#include "ECS/ECS.h"
#include <ECS/EntityManager.hpp>

namespace libCore
{
    class ScriptMoveTest : public Script {
    public:
        void Init() override {
            // Inicialización del script
            std::cout << "PASAMOS POR INIT de ScriptMoveTest" << std::endl;
        }

        void Update(float deltaTime) override 
        {
            auto& transform = EntityManager::GetInstance().GetComponent< TransformComponent>(m_Entity);

           // if (HasComponent<TransformComponent>()) {
           //     auto& transform = EntityManager::GetInstance().GetComponent< TransformComponent>(m_Entity);
           //     transform.transform->position.x += 0.1f * deltaTime;
           //     std::cout << "El componente TransformComponent Existe en esta entidad." << std::endl;
           // }
           // else {
           //     std::cout << "El componente TransformComponent NO existe en esta entidad." << std::endl;
           // }
           //* if (m_Registry && HasComponent<TransformComponent>()) 
           // {
           //     GetComponent<TransformComponent>().transform->position.x += 0.1f * deltaTime;
           // }*/
        }
    };
}
