//#pragma once
//
//#include <ECS/ECS.h>
//
//class MyScript : public libCore::Script {
//
//public:
//    void Init() override {
//        // Inicializaci�n del script
//    }
//
//    void Update(float deltaTime) override {
//        // L�gica de actualizaci�n del script
//        if (m_Registry && m_Registry->has<libCore::TransformComponent>(m_Entity)) {
//            //auto& transform = m_Registry->get<TransformComponent>(m_Entity).transform;
//            auto& transform = GetComponent<libCore::TransformComponent>().transform;
//            // Modificar la posici�n como ejemplo
//            transform->position.x += 0.01f * deltaTime;
//        }
//    }
//};