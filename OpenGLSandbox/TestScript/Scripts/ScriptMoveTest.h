#pragma once

#ifdef MY_DLL_EXPORTS
#define MY_DLL_API __declspec(dllexport)
#else
#define MY_DLL_API __declspec(dllimport)
#endif

#include <ECS/EntityManager.h>

namespace libCore
{
    class ScriptMoveTest : public Script {
    public:
        void Init() override 
        {
            std::cout << "--------------------------INIT SCRIPT MOVETEST-----------------------" << std::endl;

            std::cout << "Inicializando script para la entidad: " << static_cast<uint32_t>(m_Entity) << std::endl;

            m_Registry = EntityManager::GetInstance().GetRegistry();
            if (!m_Registry) {
                std::cerr << "Error: m_Registry es nulo en Init de ScriptMoveTest" << std::endl;
                return;
            }

            std::cout << "Accediendo a m_registry en EntityManager: " << EntityManager::GetInstance().GetRegistry() << std::endl;
            std::cout << "Direccion de m_Registry en ScriptMoveTest: " << m_Registry << std::endl;

            if (m_Registry != EntityManager::GetInstance().GetRegistry()) {
                std::cerr << "Error: m_Registry en ScriptMoveTest no coincide con m_Registry en EntityManager" << std::endl;
                return;
            }

            std::cout << "Verificando m_Registry: Número de entidades registradas: " << m_Registry->size() << std::endl;

            std::cout << "ScriptMoveTest -> Direccion de m_Entity: " << static_cast<uint32_t>(m_Entity) << std::endl;

            if (!m_Registry->valid(m_Entity)) {
                std::cerr << "Error: m_Entity no es válida dentro de m_Registry en ScriptMoveTest" << std::endl;
                return;
            }

            std::cout << "Entidad válida en m_Registry (Init)." << std::endl;
            std::cout << "------------------------------------------------------------------------" << std::endl;
        }

        void Update(float deltaTime) override {
            std::cout << "--------------------------UPDATE SCRIPT MOVETEST----------------------" << std::endl;
            std::cout << "LOOP" << std::endl;

            if (!m_Registry) {
                std::cerr << "Error: m_Registry es nulo en Update de ScriptMoveTest" << std::endl;
                return;
            }

            std::cout << "Verificando m_Registry en Update: Número de entidades registradas: " << m_Registry->size() << std::endl;

            if (!m_Registry->valid(m_Entity)) {
                std::cerr << "Error: m_Entity no es válida dentro de m_Registry en ScriptMoveTest durante la actualización" << std::endl;
                return;
            }

            std::cout << "Entidad válida en m_Registry (Update)." << std::endl;
            std::cout << "------------------------------------------------------------------------" << std::endl;
        }
    };
}


//#pragma once
//
//#ifdef MY_DLL_EXPORTS
//#define MY_DLL_API __declspec(dllexport)
//#else
//#define MY_DLL_API __declspec(dllimport)
//#endif
//
//#include <ECS/EntityManager.h>
//
//namespace libCore
//{
//    class ScriptMoveTest : public Script {
//    public:
//        void Init() override {
//            std::cout << "------------------------INIT DE ENTIDAD: " << static_cast<uint32_t>(m_Entity) << std::endl;
//
//            m_Registry = EntityManager::GetInstance().GetRegistry();
//            if (!m_Registry) {
//                std::cerr << "Error: m_Registry es nulo en Init de ScriptMoveTest" << std::endl;
//                return;
//            }
//
//            std::cout << "Accediendo a m_registry en EntityManager: " << EntityManager::GetInstance().GetRegistry() << std::endl;
//            std::cout << "Direccion de m_Registry en ScriptMoveTest: " << m_Registry << std::endl;
//
//            if (m_Registry != EntityManager::GetInstance().GetRegistry()) {
//                std::cerr << "Error: m_Registry en ScriptMoveTest no coincide con m_Registry en EntityManager" << std::endl;
//                return;
//            }
//
//            std::cout << "Verificando m_Registry: Número de entidades registradas: " << m_Registry->size() << std::endl;
//
//
//
//            std::cout << "ScriptMoveTest -> Direccion de m_Entity: " << static_cast<uint32_t>(m_Entity) << std::endl;
//
//            if (!m_Registry->valid(m_Entity)) {
//                std::cerr << "Error: m_Entity no es válida dentro de m_Registry en ScriptMoveTest" << std::endl;
//                return;
//            }
//
//            std::cout << "Entidad válida en m_Registry." << std::endl;
//
//            std::cout << "-----------------------------------------------------------------------------------------------" << std::endl;
//
//
//        }
//
//        void Update(float deltaTime) override {
//            std::cout << "LOOP" << std::endl;
//        }
//    };
//
//}
//
//
//
//// Aquí puedes seguir con el resto de la lógica que necesitas ejecutar en Init
//// Por ejemplo, acceder a un componente:
//// auto& tagComponent = GetComponent<TagComponent>();
//// std::cout << "TAG-> " << tagComponent.Tag << std::endl;
