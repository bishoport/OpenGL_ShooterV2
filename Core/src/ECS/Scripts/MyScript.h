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
            if (m_Registry &&  HasComponent<TransformComponent>()) {
                auto& transform = GetComponent<TransformComponent>().transform;
                if (InputManager::Instance().IsKeyPressed(GLFW_KEY_M))
                {
                    GetComponent<TransformComponent>().transform->position.x += 0.1f * deltaTime;
                }
            }
        }
    };
}

