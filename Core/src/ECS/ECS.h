//#pragma once
//
//#include "../Core/Transform.h"
//#include "../Core/Material.h"
//#include "../Core/Mesh.h"
//#include "../Core/Model.h"
//#include "../Core/Light.hpp"
////#include <entt.hpp>
//#include <string>
//#include <unordered_map>
//#include <functional>
//#include "../Core/Lights/DirectionalLight.h"
//#include "../Core/AABB.h"
//#include "../Core/UUID.h"
//#include "../tools/Camera.h"
////#include "Script.h"
//
//
//namespace libCore
//{
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
//            if (!registry->valid(entity)) {
//                std::cerr << "Error: La entidad no es válida en SetEntity." << std::endl;
//                return;
//            }
//            m_Entity = entity;
//            m_Registry = registry;
//        }
//
//        //// Métodos para gestionar componentes
//        /*template<typename T>
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
//        }*/
//
//        template<typename T>
//        T& GetComponent() {
//            if (!m_Registry) {
//                std::cerr << "m_Registry es nullptr en GetComponent" << std::endl;
//            }
//            if (m_Entity == entt::null) {
//                std::cerr << "m_Entity es null en GetComponent" << std::endl;
//            }
//
//            return m_Registry->get<T>(m_Entity);
//        }
//
//
//        template<typename T>
//        bool HasComponent() {
//            return m_Registry->has<T>(m_Entity);
//        }
//
//        template<typename T, typename... Args>
//        T& AddComponent(Args&&... args) {
//            return m_Registry->emplace<T>(m_Entity, std::forward<Args>(args)...);
//        }
//
//    protected:
//        entt::entity m_Entity;
//        Ref<entt::registry> m_Registry = nullptr;
//    };
//
//    class ScriptFactory {
//
//    public:
//        using ScriptCreator = std::function<Ref<Script>()>;
//
//        static ScriptFactory& GetInstance() {
//            static ScriptFactory instance;
//            return instance;
//        }
//
//        template<typename ScriptType>
//        void RegisterScript(const std::string& scriptName) {
//            creators[scriptName] = []() -> Ref<Script> {
//                return CreateRef<ScriptType>();
//            };
//        }
//
//        Ref<Script> CreateScript(const std::string& scriptName) {
//            auto it = creators.find(scriptName);
//            if (it != creators.end()) {
//                return it->second();
//            }
//            return nullptr;
//        }
//
//        const std::unordered_map<std::string, ScriptCreator>& GetCreators() const {
//            return creators;
//        }
//
//    private:
//        std::unordered_map<std::string, ScriptCreator> creators;
//
//        ScriptFactory() {}
//        ScriptFactory(const ScriptFactory&) = delete;
//        ScriptFactory& operator=(const ScriptFactory&) = delete;
//    };
//}