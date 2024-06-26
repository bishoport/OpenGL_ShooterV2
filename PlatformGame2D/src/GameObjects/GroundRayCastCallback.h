#pragma once

#include <box2d/include/box2d/box2d.h>



namespace Game
{
    class GroundRayCastCallback : public b2RayCastCallback {
    public:
        bool hit;

        GroundRayCastCallback() : hit(false) {}

        glm::float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, glm::float32 fraction) override {
            // Ignorar el propio cuerpo del jugador, si es necesario
            hit = true;
            return fraction;
        }
    };
}
