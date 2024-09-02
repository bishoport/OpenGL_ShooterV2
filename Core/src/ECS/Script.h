//#pragma once
//#include "EntityManager.hpp"
//#include "../Core/Transform.h"
//#include "../Core/Material.h"
//#include "../Core/Mesh.h"
//#include "../Core/Model.h"
//#include "../Core/Light.hpp"
//#include <entt.hpp>
//
//
//namespace libCore
//{
//
//    class Script {
//
//    public:
//        virtual ~Script() = default;
//
//        // Métodos del ciclo de vida
//        virtual void Init() {}
//        virtual void Update(float deltaTime) {}
//
//        // Métodos para establecer la entidad y el registro
//        void SetEntity(entt::entity entity, Ref<entt::registry> registry) {
//            m_Entity = entity;
//            m_Registry = registry;
//        }
//
//        //// Métodos para gestionar componentes
//        template<typename T>
//        T& GetComponent() {
//            return EntityManager::GetInstance().GetComponent<T>(m_Entity);
//        }
//
//        template<typename T>
//        bool HasComponent() {
//            return EntityManager::GetInstance().HasComponent<T>(m_Entity);
//        }
//
//        template<typename T, typename... Args>
//        T& AddComponent(Args&&... args) {
//            return EntityManager::GetInstance().AddComponent<T>(m_Entity, std::forward<Args>(args)...);
//        }
//        //template<typename T>
//        //T& GetComponent() {
//        //    return m_Registry->get<T>(m_Entity);
//        //}
//
//        //template<typename T>
//        //bool HasComponent() {
//        //    return m_Registry->has<T>(m_Entity);
//        //}
//
//        //template<typename T, typename... Args>
//        //T& AddComponent(Args&&... args) {
//        //    return m_Registry->emplace<T>(m_Entity, std::forward<Args>(args)...);
//        //}
//
//    protected:
//        entt::entity m_Entity;
//        Ref<entt::registry> m_Registry;
//    };
//}