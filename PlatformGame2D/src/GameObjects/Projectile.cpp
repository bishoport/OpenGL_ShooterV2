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
        bodyDef.bullet = true; // Mejora la detecci�n de colisiones para objetos de alta velocidad
        bodyDef.gravityScale = 0.0f;
        body = world->CreateBody(&bodyDef);


        // Configurar la forma y las propiedades del proyectil
        b2CircleShape circleShape;
        circleShape.m_radius = 0.2f; // Ajusta seg�n el tama�o deseado para tu proyectil

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &circleShape;
        fixtureDef.density = 1.0f;
        fixtureDef.restitution = 0.1f; // Restituci�n baja para un proyectil
        body->CreateFixture(&fixtureDef);
        body->SetLinearVelocity(velocity);// Aplica la velocidad inicial al proyectil

        // Guarda la direcci�n inicial normalizada para usar en la actualizaci�n
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
            // Aplica la fuerza en la direcci�n inicial del misil
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

            // Posici�n del atributo de v�rtice
            glEnableVertexAttribArray(0);
            // Ahora pasamos 3 floats para cada v�rtice y no hay hueco entre los datos (stride de 3*sizeof(float))
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
            glBindVertexArray(0); // Unbind VAO
        }

        // Obt�n la posici�n y la rotaci�n del b2Body
        b2Vec2 bodyPosition = body->GetPosition();
        glm::float32 bodyAngle = body->GetAngle();
        glm::vec3 graphicsPosition(bodyPosition.x, bodyPosition.y, 0.0f);

        m_position.x = graphicsPosition.x;
        m_position.y = graphicsPosition.y;

        // Calcula la transformaci�n para cada quad aqu�
        //glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, graphicsPosition); // Mueve el quad a la posici�n del cuerpo
        model = glm::rotate(model, bodyAngle, glm::vec3(0.0f, 0.0f, 1.0f)); // Rota el quad seg�n el �ngulo del cuerpo

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
        // Asegurarse de que el vector est� vac�o.
        m_vertices.clear();

        // El �ngulo entre cada v�rtice en radianes.
        float angleIncrement = 2.0f * M_PI / n;

        // Agregar el v�rtice central para GL_TRIANGLE_FAN
        m_vertices.push_back(0.0f); // x
        m_vertices.push_back(0.0f); // y
        m_vertices.push_back(0.0f); // z (si est�s trabajando en 2D, z puede ser 0)

        // Generar cada v�rtice del pol�gono
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
