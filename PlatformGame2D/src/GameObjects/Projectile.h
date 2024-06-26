#pragma once
#include "Sprite3D.h"
#include <box2d/include/box2d/box2d.h>

namespace Game 
{
    class Projectile : public Sprite3D {

    public:
        Projectile(b2World* world, const b2Vec2& position, const b2Vec2& velocity, const b2Vec2& direccionInicial);
        virtual ~Projectile(); // Destructor virtual si necesitas limpieza específica

        virtual void Update(libCore::Timestep deltaTime) override; // Sobrescribir Update si es necesario
        virtual void Draw() override; // Sobrescribir Draw si es necesario

        bool debeSerDestruido = false; // Flag para marcar si el misil debe ser destruido
        b2Body* body;

    private:
        
        b2Vec2 direccionInicial;
        bool motorActivo;

        float fuerzaPropulsion = 10.0f;
        float tiempoMotor = 1.0f;
        float tiempoDesdeLanzamiento = 0.1f;

        float tiempoVidaMax = 2.0f; // Tiempo de vida en segundos antes de autodestruirse
        float tiempoVida = 0.0f; // Temporizador para contar el tiempo de vida del misil
        

        std::vector<float> m_vertices;
        void generatePolygonVertices(int n, float radius);
    };
}
