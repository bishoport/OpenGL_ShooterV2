#pragma once

namespace libCore
{
    class MyScript : public Script {

    public:
        void Init() override {
            // Inicialización del script
            std::cout << "PASAMOS POR INIT de MyScript" << std::endl;
        }

        void Update(float deltaTime) override {

            // Lógica de actualización del script
           
            if (m_Registry && m_Registry->has<TransformComponent>(m_Entity)) {
                //auto& transform = m_Registry->get<TransformComponent>(m_Entity).transform;
                auto& transform = GetComponent<TransformComponent>().transform;
                // Modificar la posición como ejemplo
                transform->position.x += 0.01f * deltaTime;
            }
        }
    };
}

