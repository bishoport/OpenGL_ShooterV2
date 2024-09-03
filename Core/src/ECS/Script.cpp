#include "Script.h"
#include "EntityManager.h"

namespace libCore
{
    Script::Script() 
    {
        // Constructor - implementación aquí si es necesario
    }

    Script::~Script() {}

    void Script::SetEntity(const std::string& uuid) {

        std::cerr << "--------------------------SET ENTITY EN SCRIPT BASE-----------------------" << std::endl;

        m_UUID = uuid;
        m_Registry = EntityManager::GetInstance().GetRegistry();
        m_Entity = EntityManager::GetInstance().GetEntityByUUID(uuid);

        if (m_Registry != EntityManager::GetInstance().GetRegistry()) {
            std::cerr << "Error: m_Registry en Script no coincide con m_Registry en EntityManager" << std::endl;
            return;
        }

        if (!m_Registry) {
            std::cerr << "Error: m_Registry es nulo en SetEntity" << std::endl;
            return;
        }

        try {
            std::cout << "Verificando m_Registry: Número de entidades registradas: " << m_Registry->size() << std::endl;
            if (!m_Registry->valid(m_Entity)) {
                std::cerr << "Error: m_Entity no es válida dentro de m_Registry" << std::endl;
                return;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Excepción al acceder a m_Registry: " << e.what() << std::endl;
            return;
        }

        std::cout << "Direccion de m_Registry: " << m_Registry << std::endl;
        std::cout << "Direccion de m_Entity en SetEntity: " << static_cast<uint32_t>(m_Entity) << std::endl;

        if (m_Entity == entt::null) {
            std::cerr << "Error: m_Entity es nulo en SetEntity" << std::endl;
            return;
        }

        auto& tagComponent = EntityManager::GetInstance().GetComponent<TagComponent>(m_Entity);
        std::cout << "TAG DESDE SCRIPT-> " << tagComponent.Tag << std::endl;

        std::cerr << "----------------------------------------------------------------------------------" << std::endl;
        std::cerr << "" << std::endl;
    }



    //void Script::SetEntity(const std::string& uuid) {

    //    std::cout << "UUID en Script-> " << uuid << std::endl;

    //    m_UUID = uuid;
    //    m_Registry = EntityManager::GetInstance().GetRegistry();
    //    //m_Registry = EntityManager::GetInstance().GetRegistry().get();
    //    m_Entity = EntityManager::GetInstance().GetEntityByUUID(uuid);

    //    if (HasComponent<TagComponent>())
    //    {
    //        auto& tagComponent = GetComponent<TagComponent>();
    //        std::cout << "TAG DESDE SCRIPT-> " << tagComponent.Tag << std::endl;
    //    }
    //}
}
