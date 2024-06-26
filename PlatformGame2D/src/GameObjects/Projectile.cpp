#include "Projectile.h"
#include <tools/ShaderManager.h>

namespace Game 
{

    Projectile::Projectile(b2World* world, const b2Vec2& position, const b2Vec2& velocity, const b2Vec2& direccionInicial)
        : Sprite3D(), 
        direccionInicial(direccionInicial), 
        tiempoVida(0.0f), 
        debeSerDestruido(false), 
        motorActivo(true) {

        // Configurar el cuerpo del proyectil en Box2D
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = position;
        bodyDef.bullet = true; // Mejora la detección de colisiones para objetos de alta velocidad
        bodyDef.gravityScale = 0.0f;
        body = world->CreateBody(&bodyDef);


        // Configurar la forma y las propiedades del proyectil
        b2CircleShape circleShape;
        circleShape.m_radius = 0.2f; // Ajusta según el tamaño deseado para tu proyectil

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &circleShape;
        fixtureDef.density = 1.0f;
        fixtureDef.restitution = 0.1f; // Restitución baja para un proyectil
        body->CreateFixture(&fixtureDef);
        body->SetLinearVelocity(velocity);// Aplica la velocidad inicial al proyectil

        // Guarda la dirección inicial normalizada para usar en la actualización
        this->direccionInicial.Normalize();

        generatePolygonVertices(5, 0.2f);
    }


    Projectile::~Projectile()
    {

    }

    void Projectile::Update(libCore::Timestep deltaTime)
    {
        tiempoVida += deltaTime;
        if (tiempoVida >= tiempoVidaMax || debeSerDestruido) {
            debeSerDestruido = true;
        }

        tiempoDesdeLanzamiento += deltaTime;
        if (motorActivo && tiempoDesdeLanzamiento < tiempoMotor) {
            // Aplica la fuerza en la dirección inicial del misil
            b2Vec2 fuerza = b2Vec2(direccionInicial.x * fuerzaPropulsion, direccionInicial.y * fuerzaPropulsion);
            body->ApplyForceToCenter(fuerza, true);
        }
        else 
        {
            motorActivo = false;
        }
    }


    void Projectile::Draw()
    {
        if (quadVAO == 0)
        {
            // Setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            
            glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);

            // Posición del atributo de vértice
            glEnableVertexAttribArray(0);
            // Ahora pasamos 3 floats para cada vértice y no hay hueco entre los datos (stride de 3*sizeof(float))
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
            glBindVertexArray(0); // Unbind VAO
        }

        // Obtén la posición y la rotación del b2Body
        b2Vec2 bodyPosition = body->GetPosition();
        glm::float32 bodyAngle = body->GetAngle();
        glm::vec3 graphicsPosition(bodyPosition.x, bodyPosition.y, 0.0f);

        m_position.x = graphicsPosition.x;
        m_position.y = graphicsPosition.y;

        // Calcula la transformación para cada quad aquí
        //glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, graphicsPosition); // Mueve el quad a la posición del cuerpo
        model = glm::rotate(model, bodyAngle, glm::vec3(0.0f, 0.0f, 1.0f)); // Rota el quad según el ángulo del cuerpo

        // Establece la matriz de modelo en tu shader
        libCore::ShaderManager::Get("base")->use();
        libCore::ShaderManager::Get("base")->setMat4("model", model);
        libCore::ShaderManager::Get("base")->setBool("use_texture", false); // Asumiendo que no usas texturas por ahora
        glm::vec4 myColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        libCore::ShaderManager::Get("base")->setVec4("u_Color", myColor);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, m_vertices.size() / 3);
        glBindVertexArray(0);
    }

    void Projectile::generatePolygonVertices(int n, float radius)
    {
        // Asegurarse de que el vector esté vacío.
        m_vertices.clear();

        // El ángulo entre cada vértice en radianes.
        float angleIncrement = 2.0f * M_PI / n;

        // Agregar el vértice central para GL_TRIANGLE_FAN
        m_vertices.push_back(0.0f); // x
        m_vertices.push_back(0.0f); // y
        m_vertices.push_back(0.0f); // z (si estás trabajando en 2D, z puede ser 0)

        // Generar cada vértice del polígono
        for (int i = 0; i <= n; ++i) 
        { // Usar <= n para volver al punto de inicio
            float angle = i * angleIncrement;
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            m_vertices.push_back(x);
            m_vertices.push_back(y);
            m_vertices.push_back(0.0f); // z
        }
    }

}
