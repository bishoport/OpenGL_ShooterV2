#pragma once

namespace libCore
{
    template<typename T>
    T& Script::GetComponent() {
        std::cout << "Script GetComponent-> Direccion de m_Entity: " << static_cast<uint32_t>(m_Entity) << std::endl;
        return EntityManager::GetInstance().GetComponent<T>(m_Entity);
    }

    template<typename T>
    bool Script::HasComponent() {
        return EntityManager::GetInstance().HasComponent<T>(m_Entity);
    }
}


//#pragma once
//
//namespace libCore
//{
//    template<typename T>
//    T& Script::GetComponent() {
//        //if (!m_Registry) {
//        //    throw std::runtime_error("Intento de acceder a un componente con un registro nulo.");
//        //}
//        //if (m_Entity == entt::null || !m_Registry->valid(m_Entity)) {
//        //    throw std::runtime_error("Intento de acceder a un componente con una entidad nula o inválida.");
//        //}
//        return EntityManager::GetInstance().GetComponent<T>(EntityManager::GetInstance().GetEntityByUUID(m_UUID));
//        //return m_Registry->get<T>(m_Entity);
//    }
//
//    template<typename T>
//    bool Script::HasComponent() {
//        return EntityManager::GetInstance().HasComponent<T>(EntityManager::GetInstance().GetEntityByUUID(m_UUID));
//        //return m_Entity != entt::null && m_Registry->valid(m_Entity) && m_Registry->has<T>(m_Entity);
//    }
//}
