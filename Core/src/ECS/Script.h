#pragma once

#include "../LibCoreHeaders.h"
#include <entt.hpp>
#include <memory>
#include <string>


#ifdef MY_DLL_EXPORTS
#define MY_DLL_API __declspec(dllexport)
#else
#define MY_DLL_API __declspec(dllimport)
#endif

#pragma warning(disable: 4251)



namespace libCore
{
    class EntityManager;

    class MY_DLL_API  Script {

    public:
        Script();  // Declaración del constructor
        ~Script();  // Declaración del destructor

        // Métodos del ciclo de vida
        virtual void Init() {}
        virtual void Update(float deltaTime) {}

        // Métodos para establecer el UUID y el registro
        void SetEntity(const std::string& uuid);

        template<typename T>
        T& GetComponent();

        template<typename T>
        bool HasComponent();

    //protected:
        std::string m_UUID;
        entt::entity m_Entity = entt::null;
        //Ref<entt::registry> m_Registry = nullptr;
        entt::registry* m_Registry = nullptr;
    };
}

#include "Script.inl"
