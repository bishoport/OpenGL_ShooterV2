#pragma once

namespace libCore
{
    class MyScript : public Script {

    public:
        void Init() override {
            // Inicializaci�n del script
            std::cout << "PASAMOS POR INIT de MyScript" << std::endl;
        }

        void Update(float deltaTime) override {

            // L�gica de actualizaci�n del script
           
            if (m_Registry && m_Registry->has<TransformComponent>(m_Entity)) {
                //auto& transform = m_Registry->get<TransformComponent>(m_Entity).transform;
                auto& transform = GetComponent<TransformComponent>().transform;
                // Modificar la posici�n como ejemplo
                transform->position.x += 0.01f * deltaTime;
            }
        }
    };
}

