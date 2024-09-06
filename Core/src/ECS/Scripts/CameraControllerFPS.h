#pragma once

namespace libCore
{
    class CameraControllerFPS  {

    public:
        bool isJumping = false;
        float jumpSpeed = 10.0f;
        float gravity = -19.81f;
        float verticalVelocity = 0.0f;
        float floorLimit = 0.0f;

        Ref<Transform> transform = nullptr;
        CameraComponent cameraComponent;

        //void Init() override
        //{
        //    //transform = GetComponent<TransformComponent>().transform;
        //    //cameraComponent = GetComponent<CameraComponent>();
        //    //floorLimit = transform->position.y;
        //}

        //void Update(float deltaTime) override {

        //    // Logica de entradas para la cámara FPS
        //    float moveSpeed = cameraComponent.camera->speed * deltaTime;

        //    // Movimiento adelante y atrás
        //    glm::vec3 forward = glm::normalize(glm::vec3(cameraComponent.camera->Orientation.x, 0.0f, cameraComponent.camera->Orientation.z));
        //    if (InputManager::Instance().IsKeyPressed(GLFW_KEY_W))
        //    {
        //        transform->position += moveSpeed * forward;
        //    }

        //    if (InputManager::Instance().IsKeyPressed(GLFW_KEY_S))
        //    {
        //        transform->position -= moveSpeed * forward;
        //    }


        //    // Movimiento izquierda y derecha
        //    glm::vec3 right = glm::normalize(glm::cross(forward, cameraComponent.camera->Up));
        //    if (InputManager::Instance().IsKeyPressed(GLFW_KEY_A))
        //        transform->position -= moveSpeed * right;
        //    if (InputManager::Instance().IsKeyPressed(GLFW_KEY_D))
        //        transform->position += moveSpeed * right;

        //    // Manejo de saltos
        //    if (InputManager::Instance().IsKeyPressed(GLFW_KEY_SPACE) && !isJumping)
        //    {
        //        isJumping = true;
        //        verticalVelocity = jumpSpeed;
        //    }

        //    // Aplicar gravedad
        //    if (isJumping)
        //    {
        //        verticalVelocity += gravity * deltaTime;
        //        transform->position.y += verticalVelocity * deltaTime;

        //        // Si toca el suelo
        //        if (transform->position.y <= floorLimit)
        //        {
        //            transform->position.y = floorLimit;
        //            isJumping = false;
        //            verticalVelocity = 0.0f;
        //        }
        //    }

        //    // Manejo del ratón para rotar la cámara
        //    glfwSetInputMode(libCore::EngineOpenGL::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        //    double mouseX, mouseY;
        //    glfwGetCursorPos(libCore::EngineOpenGL::GetWindow(), &mouseX, &mouseY);

        //    float rotX = cameraComponent.camera->sensitivity * (float)(mouseY - (cameraComponent.camera->height / 2)) / cameraComponent.camera->height;
        //    float rotY = cameraComponent.camera->sensitivity * (float)(mouseX - (cameraComponent.camera->width / 2)) / cameraComponent.camera->width;

        //    // Aplicar rotaciones a la orientación de la cámara
        //    cameraComponent.camera->Orientation = glm::rotate(cameraComponent.camera->Orientation, glm::radians(-rotX), right);
        //    cameraComponent.camera->Orientation = glm::rotate(cameraComponent.camera->Orientation, glm::radians(-rotY), cameraComponent.camera->Up);

        //    // Volver a centrar el cursor
        //    glfwSetCursorPos(libCore::EngineOpenGL::GetWindow(), (cameraComponent.camera->width / 2), (cameraComponent.camera->height / 2));

        //    // Actualizar la posición de la cámara en base a la transform
        //    cameraComponent.camera->Position = transform->position;
        //    cameraComponent.camera->updateMatrix();
        //}
    };
}
