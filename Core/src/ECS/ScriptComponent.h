#pragma once

#include "Script.h"
#include <memory>
#include <iostream> // Para la salida de depuración

namespace libCore
{
    struct ScriptComponent {

        Ref<Script> instance = nullptr;

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent&) = default;
        ScriptComponent(Ref<Script> script) : instance(script) {}

        void Init() {
            std::cerr << "--------------------------INIT SCRIPT COMPONENT-----------------------" << std::endl;
            if (instance)
            {
                if (!instance->m_Registry) {
                    std::cerr << "Error: m_Registry es nulo en el script durante la inicialización." << std::endl;
                }
                else {
                    std::cout << "Direccion de m_Registry en ScriptComponent (Init): " << instance->m_Registry << std::endl;
                    std::cout << "Numero de entidades en m_Registry (Init): " << instance->m_Registry->size() << std::endl;
                }

                if (instance->m_Entity == entt::null) {
                    std::cerr << "Error: La entidad en el script es nula durante la inicialización." << std::endl;
                }
                else {
                    if (instance->m_Registry->valid(instance->m_Entity)) {
                        std::cout << "Entidad válida en m_Registry (Init). Entidad ID: " << static_cast<uint32_t>(instance->m_Entity) << std::endl;
                    }
                    else {
                        std::cerr << "Error: m_Entity no es válida dentro de m_Registry durante la inicialización." << std::endl;
                    }
                }
                instance->Init();
            }
            else {
                std::cerr << "Error: El script instance es nulo durante la inicialización." << std::endl;
            }
            std::cerr << "------------------------------------------------------------------------" << std::endl;
        }

        void Update(float deltaTime) {
            std::cerr << "--------------------------UPDATE SCRIPT COMPONENT----------------------" << std::endl;
            if (instance)
            {
                if (!instance->m_Registry) {
                    std::cerr << "Error: m_Registry es nulo en el script durante la actualización." << std::endl;
                }
                else {
                    std::cout << "Direccion de m_Registry en ScriptComponent (Update): " << instance->m_Registry << std::endl;
                    std::cout << "Numero de entidades en m_Registry (Update): " << instance->m_Registry->size() << std::endl;
                }

                if (instance->m_Entity == entt::null) {
                    std::cerr << "Error: La entidad en el script es nula durante la actualización." << std::endl;
                }
                else {
                    if (instance->m_Registry->valid(instance->m_Entity)) {
                        std::cout << "Entidad válida en m_Registry (Update). Entidad ID: " << static_cast<uint32_t>(instance->m_Entity) << std::endl;
                    }
                    else {
                        std::cerr << "Error: m_Entity no es válida dentro de m_Registry durante la actualización." << std::endl;
                    }
                }
                instance->Update(deltaTime);
            }
            else {
                std::cerr << "Error: El script instance es nulo durante la actualización." << std::endl;
            }
            std::cerr << "------------------------------------------------------------------------" << std::endl;
        }
    };
}




//#pragma once
//
//#include "Script.h"
//#include <memory>
//
//namespace libCore
//{
//    struct ScriptComponent {
//
//        Ref<Script> instance = nullptr;
//
//        ScriptComponent() = default;
//        ScriptComponent(const ScriptComponent&) = default;
//        ScriptComponent(Ref<Script> script) : instance(script) {}
//
//        void Init() {
//            if (instance) {
//                instance->Init();
//            }
//        }
//
//        void Update(float deltaTime) {
//            if (instance) {
//                instance->Update(deltaTime);
//            }
//        }
//    };
//}
