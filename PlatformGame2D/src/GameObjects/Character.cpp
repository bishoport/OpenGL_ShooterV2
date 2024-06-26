#include "Character.h"
#include <input/InputManager.h>
#include "tools/TextureManager.h"
#include <tools/ShaderManager.h>
#include "GroundRayCastCallback.h"

namespace Game
{
    // Constructor que acepta un objeto CharacterData
    Character::Character(const CharacterData& data, b2World* world) : m_characterData(data), m_world(world)
    {
        // Definición del cuerpo Box2D
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(m_position.x, m_position.y);
        m_body = world->CreateBody(&bodyDef);

        // Define la forma del cuerpo
        b2CircleShape circleCollider;
        circleCollider.m_p.Set(0.0f, -0.0f);
        circleCollider.m_radius = 0.5f;

        // Define las propiedades físicas
        b2FixtureDef fixtureDef;
        //fixtureDef.shape = &dynamicBox;
        fixtureDef.shape = &circleCollider;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.3f;
        fixtureDef.restitution = 0.0f; // Bajo para minimizar el rebote
        bodyDef.linearDamping = 0.1f; // Reduce gradualmente la velocidad de movimiento
        bodyDef.angularDamping = 0.1f; // Reduce gradualmente la velocidad de rotación



        m_body->CreateFixture(&fixtureDef);

        std::string texturesDirectory = "Assets/Sprites/";

        animations[CharacterState::Idle] = Animation{
            "Idle",
            libCore::TextureManager::LoadTexture((texturesDirectory + "idle.png").c_str(), "diffuse", 0),
            15, // Supongamos que hay 10 frames
            0, // Frame inicial
            0.1f,//Frame Time
            256.0f / 4096.0f, // Ancho del frame en coordenadas de textura
            true // Loop
        };

        animations[CharacterState::Run] = Animation{
            "Run",
            libCore::TextureManager::LoadTexture((texturesDirectory + "run.png").c_str(), "diffuse", 0),
            15, // Supongamos que hay 8 frames
            0, // Frame inicial
            0.05f,//Frame Time
            256.0f / 4096.0f, // Ancho del frame en coordenadas de textura
            true // Loop
        };

        animations[CharacterState::Idle].texture->Bind("");
        animations[CharacterState::Run].texture->Bind("");

        currentState = CharacterState::Idle;


        //INIT QUAD VAO-VBO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

        // Atributo de posición de vértice
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        // Atributo de coordenadas UV
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
        glBindVertexArray(0); // Unbind VAO
    }

    Character::~Character(){}

    void Character::Update(libCore::Timestep deltaTime)
    {
        isMoving = false;

        //--GROUND CONTROL
        GroundRayCastCallback groundCallback;
        b2Vec2 startPoint = m_body->GetPosition();
        b2Vec2 endPoint = startPoint + b2Vec2(0, -1.0f); // Raycast hacia abajo 1 metro
        m_world->RayCast(&groundCallback, startPoint, endPoint);
        m_onGround = groundCallback.hit;
        //// Raycast hacia arriba
        //b2Vec2 endPointUp = startPoint + b2Vec2(0, 1);
        //// Raycast hacia la izquierda
        //b2Vec2 endPointLeft = startPoint + b2Vec2(-1, 0);
        //// Raycast hacia la derecha
        //b2Vec2 endPointRight = startPoint + b2Vec2(1, 0);
        //---------------------------------------------------------------------------------


        //--LEFT RIGHT MOVE
        b2Vec2 velocity = m_body->GetLinearVelocity();
        float desiredVelocity = 0;

        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_A)) 
        {
            desiredVelocity = b2Max(velocity.x - 0.1f * acceleration, +maxSpeed);
            isMoving = true;
            lookingToTheRight = false;
        }
        else if (InputManager::Instance().IsKeyPressed(GLFW_KEY_D)) 
        {
            desiredVelocity = b2Min(velocity.x + 0.1f * acceleration, -maxSpeed);
            isMoving = true;
            lookingToTheRight = true;
        }

        // Disparo de proyectil
        if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_Q)) {
            FireProjectile(45.0f);
        }
        else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_E)) {
            FireProjectile(-45.0f);
        }

        float velocityChange = desiredVelocity - velocity.x;
        float impulse = m_body->GetMass() * velocityChange; // masa * cambio de velocidad = impulso
        m_body->ApplyLinearImpulseToCenter(b2Vec2(impulse, 0), true);
        //---------------------------------------------------------------------------------


        //--JUMP
        if (m_onGround && InputManager::Instance().IsKeyJustPressed(GLFW_KEY_SPACE) && m_onGround) 
        {
             m_body->ApplyLinearImpulseToCenter(b2Vec2(0, m_body->GetMass() * jumpForce), true);
            m_onGround = false; // Necesitarás una lógica para actualizar esto basado en colisiones
        }
        //---------------------------------------------------------------------------------

        //--DASH
        //if (!isDashing && InputManager::Instance().IsKeyJustPressed(GLFW_KEY_LEFT_SHIFT) && (m_world->GetTime() - lastDashTime > dashCooldown)) {
        //    isDashing = true;
        //    lastDashTime = m_world->GetTime();

        //    float dashDirection = lookingToTheRight ? 1.0f : -1.0f; // Dash hacia la derecha o izquierda basado en hacia dónde mira el personaje
        //    b2Vec2 dashImpulse = b2Vec2(dashForce * dashDirection, 0);
        //    m_body->ApplyLinearImpulseToCenter(dashImpulse, true);
        //}
        //---------------------------------------------------------------------------------
        // 
        //-- UPDATE POSITION
        b2Vec2 position = m_body->GetPosition();
        m_position.x = position.x;
        m_position.y = position.y;
        //---------------------------------------------------------------------------------


        //-- CURRENT CHARACTER STATE ANIMATION
        if (isMoving)
            ChangeState(CharacterState::Run);
        else
            ChangeState(CharacterState::Idle);

        auto& anim = animations[currentState];
        timeSinceLastFrame += deltaTime;

        if (timeSinceLastFrame >= anim.frameTime) 
        {
            if (anim.loop || (!anim.loop && anim.currentFrame < anim.totalFrames - 1))
            {
                anim.currentFrame = (anim.currentFrame + 1) % anim.totalFrames;
            }
            timeSinceLastFrame -= anim.frameTime;
        }
        //---------------------------------------------------------------------------------


        //--PROYECTILES
        for (auto it = projectiles.begin(); it != projectiles.end(); ) {
            (*it)->Update(deltaTime);
            if ((*it)->debeSerDestruido) 
            {
                m_world->DestroyBody((*it)->body);
                delete* it; // Llama al destructor del proyectil
                it = projectiles.erase(it); // Elimina el proyectil del vector
            }
            else {
                ++it;
            }
        }
        //---------------------------------------------------------------------------------
    }


    void Character::FireProjectile(float angleDegrees) 
    {
        // Calcula la dirección del disparo basándote en la orientación del personaje
        b2Vec2 direction = lookingToTheRight ? b2Vec2(-1.0f, 0.0f) : b2Vec2(1.0f, 0.0f);

        // La velocidad inicial del proyectil
        b2Vec2 velocity = b2Vec2(direction.x, direction.y);

        // Posición desde la cual se dispara el proyectil, ajusta según la configuración de tu personaje
        b2Vec2 position = m_body->GetPosition() + b2Vec2(direction.x * 0.5f, direction.y * 0.5f); // Este es un offset de ejemplo

        // Crear un nuevo proyectil y pasarlo la posición y velocidad inicial
        projectiles.push_back(new Projectile(m_world, position, velocity, direction));
    }


    void Character::Draw()
    {
        auto& anim = animations[currentState]; // Usa la animación actual basada en el estado

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_position);

        // Calcula las coordenadas de textura para el frame actual
        float uOffset = anim.currentFrame * anim.frameWidth;
        float uMax = uOffset + anim.frameWidth;

        // Actualiza el VBO con los vértices actuales
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        if (lookingToTheRight) {
            // Configura quadVertices para mirar a la derecha
            quadVertices[3] = uMax; // UVs
            quadVertices[8] = uMax; // UVs
            quadVertices[13] = uOffset;
            quadVertices[18] = uOffset;
        }
        else {
            // Configura quadVertices para mirar a la izquierda
            quadVertices[3] = uOffset; // UVs
            quadVertices[8] = uOffset; // UVs
            quadVertices[13] = uMax;
            quadVertices[18] = uMax;

        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); // No es estrictamente necesario desvincular cada vez

        // Configuración de la textura y shader
        glActiveTexture(GL_TEXTURE0);
        libCore::ShaderManager::Get("base")->use();
        libCore::ShaderManager::Get("base")->setInt("texture1", 0);
        glBindTexture(GL_TEXTURE_2D, anim.texture->ID);
        libCore::ShaderManager::Get("base")->setMat4("model", model);
        libCore::ShaderManager::Get("base")->setBool("use_texture", true);

        // Dibujar el quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0); // Desvincular el VAO no es necesario pero es una buena práctica

        for (auto& projectile : projectiles) {
            if (!projectile->debeSerDestruido) {
                projectile->Draw();
            }
        }
    }


    void Character::ChangeState(CharacterState newState)
    {
        if (currentState != newState) {
            currentState = newState;
            auto& anim = animations[currentState];
            anim.currentFrame = 0; // Reinicia la animación para estados de una sola reproducción
            // Ajustar si la animación debe ser en loop
            anim.loop = (newState == CharacterState::Idle || newState == CharacterState::Run);
        }
    }
}













































//#include "Character.h"
//#include <input/InputManager.h>
//#include "../ImageLoader.h"
//#include <ShaderManager.h>
//
//namespace Game
//{
//    // Constructor que acepta un objeto CharacterData
//    Character::Character(const CharacterData& data) : m_characterData(data)
//    {
//        animations[CharacterState::Idle] = Animation{
//            "Idle",
//            Game::ImageLoader::LoadTexture("C:/Users/pdortegon/Documents/PROPIOS/OpenGL_Playground/OpenGL_Playground/Platform2DGame/Assets/Sprites/idle.png"),
//            15, // Supongamos que hay 10 frames
//            0, // Frame inicial
//            0.1f,//Frame Time
//            256.0f / 4096.0f, // Ancho del frame en coordenadas de textura
//            true // Loop
//        };
//
//        animations[CharacterState::Run] = Animation{
//            "Run",
//             Game::ImageLoader::LoadTexture("C:/Users/pdortegon/Documents/PROPIOS/OpenGL_Playground/OpenGL_Playground/Platform2DGame/Assets/Sprites/run.png"),
//            15, // Supongamos que hay 8 frames
//            0, // Frame inicial
//            0.05f,//Frame Time
//            256.0f / 4096.0f, // Ancho del frame en coordenadas de textura
//            true // Loop
//        };
//
//        animations[CharacterState::Idle].texture->Bind();
//        animations[CharacterState::Run].texture->Bind();
//
//        currentState = CharacterState::Idle;
//
//
//
//        glGenVertexArrays(1, &VAO);
//        glGenBuffers(1, &VBO);
//        glBindVertexArray(VAO);
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//
//        // Atributo de posición de vértice
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//
//        // Atributo de coordenadas UV
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//
//        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
//        glBindVertexArray(0); // Unbind VAO
//    }
//
//    Character::~Character()
//    {
//    }
//
//    std::string CharacterStateToString(CharacterState state) {
//        switch (state) {
//        case CharacterState::Idle: return "Idle";
//        case CharacterState::Run: return "Run";
//        case CharacterState::Jump: return "Jump";
//        case CharacterState::Attack: return "Attack";
//        case CharacterState::Damage: return "Damage";
//        case CharacterState::Dead: return "Dead";
//        default: return "Unknown State";
//        }
//    }
//
//    void Character::Update(libopengl::Timestep deltaTime)
//    {
//        isMoving = false;
//        // Corrección en la lógica de movimiento horizontal
//        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_A)) {
//            m_velocityX += acceleration * deltaTime; // Aceleración hacia la derecha
//            m_velocityX = std::min(m_velocityX, maxSpeed); // Limitar a velocidad máxima
//            lookingToTheRight = false;
//            lookingToTheLeft = true;
//            isMoving = true;
//        }
//        else if (InputManager::Instance().IsKeyPressed(GLFW_KEY_D)) {
//            m_velocityX -= acceleration * deltaTime; // Aceleración hacia la izquierda
//            m_velocityX = std::max(m_velocityX, -maxSpeed); // Limitar a velocidad máxima
//            lookingToTheLeft = false;
//            lookingToTheRight = true;
//            isMoving = true;
//        }
//
//
//        float incrementoRay = 0.01f;
//        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_O))
//        {
//            rayEndOffset.y += incrementoRay;
//        }
//        else if (InputManager::Instance().IsKeyPressed(GLFW_KEY_L))
//        {
//            rayEndOffset.y -= incrementoRay;
//        }
//
//
//        if (InputManager::Instance().IsKeyPressed(GLFW_KEY_P))
//        {
//            rayStartOffset = glm::vec3(0.0f, -0.5f, 0.0f);
//            rayEndOffset   = glm::vec3(0.0f, -0.5f, 0.0f);
//        }
//
//        // Aplicar fricción o desaceleración
//        if (!InputManager::Instance().IsKeyPressed(GLFW_KEY_A) && !InputManager::Instance().IsKeyPressed(GLFW_KEY_D)) {
//            if (m_velocityX > 0) {
//                m_velocityX -= deceleration * deltaTime; // Desacelerar a la derecha
//                m_velocityX = std::max(m_velocityX, 0.0f); // Evitar que se invierta la dirección
//            }
//            else if (m_velocityX < 0) {
//                m_velocityX += deceleration * deltaTime; // Desacelerar a la izquierda
//                m_velocityX = std::min(m_velocityX, 0.0f); // Evitar que se invierta la dirección
//            }
//        }
//
//        // Salto
//        if (m_onGround && InputManager::Instance().IsKeyPressed(GLFW_KEY_SPACE)) {
//            m_velocityY = jumpForce; // Aplicar fuerza de salto
//            m_onGround = false; // El personaje está en el aire
//        }
//
//        if (m_onGround && InputManager::Instance().IsKeyJustPressed(GLFW_KEY_I)) {
//            std::cout << "m_position.x-> " << m_position.x << std::endl;
//            std::cout << "m_position.y-> " << m_position.y << std::endl;
//            std::cout << "m_position.z-> " << m_position.z << std::endl;
//            std::cout << "-----------------------------------------------------" << std::endl;
//
//            std::cout << "rayStart.x-> " << rayStart.x << std::endl;
//            std::cout << "rayStart.y-> " << rayStart.y << std::endl;
//            std::cout << "rayStart.z-> " << rayStart.z << std::endl;
//
//            std::cout << "-----------------------------------------------------" << std::endl;
//
//            std::cout << "rayEnd.x-> " << rayEnd.x << std::endl;
//            std::cout << "rayEnd.y-> " << rayEnd.y << std::endl;
//            std::cout << "rayEnd.z-> " << rayEnd.z << std::endl;
//
//            
//            std::cout << "-----------------------------------------------------" << std::endl;
//
//            std::cout << "currentState " << CharacterStateToString(currentState) << std::endl;
//            std::cout << "lookingToTheRight " << lookingToTheRight << std::endl;
//
//
//        }
//
//        // Aplicar gravedad
//        if (!m_onGround)
//        {
//            m_velocityY -= gravity * deltaTime; // Aplicar gravedad
//        }
//        else
//        {
//            m_velocityY = 0;
//        }
//
//        // Actualizar posición basada en velocidad
//        m_position.x += m_velocityX * deltaTime;
//        m_position.y += m_velocityY * deltaTime;
//
//
//        // Actualizar rayStart y rayEnd
//        rayStart = glm::vec3(m_position.x, m_position.y, m_position.z);
//        rayStart += rayStartOffset;
//
//        rayEnd = glm::vec3(m_position.x, m_position.y , m_position.z);
//        rayEnd += rayEndOffset;
//        
//
//
//        // Cambiar estado basado en la entrada
//        if (isMoving)
//        {
//            ChangeState(CharacterState::Run);
//        }
//        else {
//            ChangeState(CharacterState::Idle);
//        }
//
//        auto& anim = animations[currentState];
//        timeSinceLastFrame += deltaTime;
//
//        if (timeSinceLastFrame >= anim.frameTime) {
//            if (anim.loop || (!anim.loop && anim.currentFrame < anim.totalFrames - 1)) {
//                anim.currentFrame = (anim.currentFrame + 1) % anim.totalFrames;
//            }
//            timeSinceLastFrame -= anim.frameTime;
//        }
//    }
//
//    void Character::Draw()
//    {
//        auto& anim = animations[currentState]; // Usa la animación actual basada en el estado
//
//        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::translate(model, m_position);
//
//        // Calcula las coordenadas de textura para el frame actual
//        float uOffset = anim.currentFrame * anim.frameWidth;
//        float uMax = uOffset + anim.frameWidth;
//
//        // Actualiza el VBO con los vértices actuales
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        if (lookingToTheRight) {
//            // Configura quadVertices para mirar a la derecha
//            quadVertices[3] = uMax; // UVs
//            quadVertices[8] = uMax; // UVs
//            quadVertices[13] = uOffset;
//            quadVertices[18] = uOffset;
//        }
//        else {
//            // Configura quadVertices para mirar a la izquierda
//            quadVertices[3] = uOffset; // UVs
//            quadVertices[8] = uOffset; // UVs
//            quadVertices[13] = uMax;
//            quadVertices[18] = uMax;
//            
//        }
//        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
//        glBindBuffer(GL_ARRAY_BUFFER, 0); // No es estrictamente necesario desvincular cada vez
//
//        // Configuración de la textura y shader
//        glActiveTexture(GL_TEXTURE0);
//        libopengl::ShaderManager::Get("base")->use();
//        libopengl::ShaderManager::Get("base")->setInt("texture1", 0);
//        glBindTexture(GL_TEXTURE_2D, anim.texture->textureID);
//        libopengl::ShaderManager::Get("base")->setMat4("model", model);
//        libopengl::ShaderManager::Get("base")->setBool("use_texture", true);
//
//        // Dibujar el quad
//        glBindVertexArray(VAO);
//        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//        glBindVertexArray(0); // Desvincular el VAO no es necesario pero es una buena práctica
//
//        modelToLines = model;
//        DrawRay();
//    }
//
//    void Character::DrawRay()
//    {
//        // Define los vértices de la línea
//        GLfloat vertices[] = {
//            rayStart.x, rayStart.y, rayStart.z,
//            rayEnd.x, rayEnd.y, rayEnd.z
//        };
//
//        // Puedes usar un VAO y VBO para dibujar la línea
//        glGenVertexArrays(1, &VAOLine);
//        glGenBuffers(1, &VBOLine);
//   
//
//        // Configura el VAO y VBO
//        glBindVertexArray(VAOLine);
//        glBindBuffer(GL_ARRAY_BUFFER, VBOLine);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//        glEnableVertexAttribArray(0);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);
//
//
//        glm::mat4 model = glm::mat4(1.0f);
//
//        // Establece la matriz de modelo en tu shader
//        libopengl::ShaderManager::Get("debug")->use();
//        libopengl::ShaderManager::Get("debug")->setMat4("model", model);
//        glm::vec4 lineColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Color blanco para las líneas
//        libopengl::ShaderManager::Get("debug")->setVec4("u_Color", lineColor);
//
//        // Dibuja la línea
//        glBindVertexArray(VAOLine);
//        glDrawArrays(GL_LINES, 0, 2); // Dos puntos para dibujar la línea
//        glBindVertexArray(0);
//
//        // Limpia los recursos
//        glDeleteVertexArrays(1, &VAOLine);
//        glDeleteBuffers(1, &VBOLine);
//    }
//
//
//    void Character::ChangeState(CharacterState newState)
//    {
//        if (currentState != newState) {
//            currentState = newState;
//            auto& anim = animations[currentState];
//            anim.currentFrame = 0; // Reinicia la animación para estados de una sola reproducción
//            // Ajustar si la animación debe ser en loop
//            anim.loop = (newState == CharacterState::Idle || newState == CharacterState::Run);
//        }
//    }
//}
