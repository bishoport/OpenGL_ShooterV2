#include "TileQuad.h"
#include <tools/ShaderManager.h>
#include <input/InputManager.h>

namespace Game
{
	TileQuad::TileQuad(){}

    TileQuad::~TileQuad() {
        // Aseg�rate de liberar los recursos de OpenGL
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);

        // Tambi�n limpia los recursos para las l�neas de AABB
        glDeleteVertexArrays(1, &lineVAO);
        glDeleteBuffers(1, &lineVBO);
    }


    // M�todo para inicializar la f�sica de Box2D
    void TileQuad::InitializePhysics(b2World* world, float tileWidth, float tileHeight, bool pyshics) {
        b2BodyDef bodyDef;
        if (pyshics)
        {
            bodyDef.type = b2_dynamicBody; // O b2_staticBody, seg�n necesites
        }
        else
        {
            bodyDef.type = b2_staticBody; // O b2_staticBody, seg�n necesites
        }
        
        bodyDef.position.Set(m_position.x, m_position.y);

        body = world->CreateBody(&bodyDef);

        b2PolygonShape box;
        box.SetAsBox(tileWidth / 2.0f, tileHeight / 2.0f);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &box;
        fixtureDef.density = 1.0f; // Ajusta seg�n necesidad
        fixtureDef.friction = 0.6f; // Ajusta seg�n necesidad
        fixtureDef.restitution = 0.01f; // Bajo para minimizar el rebote
        bodyDef.linearDamping = 0.9f; // Reduce gradualmente la velocidad de movimiento
        bodyDef.angularDamping = 0.9f; // Reduce gradualmente la velocidad de rotaci�n

        body->CreateFixture(&fixtureDef);
    }


    void TileQuad::Update(libCore::Timestep deltaTime){}

    void TileQuad::Draw()
    {
        if (quadVAO == 0)
        {
            // Setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

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

        m_position.x  = graphicsPosition.x;
        m_position.y  = graphicsPosition.y;

        // Calcula la transformaci�n para cada quad aqu�
        //glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, graphicsPosition); // Mueve el quad a la posici�n del cuerpo
        model = glm::rotate(model, bodyAngle, glm::vec3(0.0f, 0.0f, 1.0f)); // Rota el quad seg�n el �ngulo del cuerpo

        // Establece la matriz de modelo en tu shader
        libCore::ShaderManager::Get("base")->use();
        libCore::ShaderManager::Get("base")->setMat4("model", model);
        libCore::ShaderManager::Get("base")->setBool("use_texture", false); // Asumiendo que no usas texturas por ahora
        glm::vec4 myColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        libCore::ShaderManager::Get("base")->setVec4("u_Color", myColor);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Dibuja el Quad
        glBindVertexArray(0);

        // Suponiendo que tienes m�todos para actualizar y dibujar el AABB
       //UpdateAABB();
       //DrawAABB();
    }






    void TileQuad::UpdateAABB()
    {
        centro = glm::vec3(0.0f, 0.0f, 0.0f);

        // Calcular el centro del Quad
        for (int i = 0; i < 12; i += 3) {
            centro += glm::vec3(quadVertices[i], quadVertices[i + 1], quadVertices[i + 2]);
        }
        centro /= 4.0f;

        // Calcular el AABB local
        glm::vec3 min(quadVertices[0], quadVertices[1], quadVertices[2]);
        glm::vec3 max(quadVertices[0], quadVertices[1], quadVertices[2]);

        for (int i = 0; i < 12; i += 3) {
            min.x = std::min(min.x, quadVertices[i]);
            min.y = std::min(min.y, quadVertices[i + 1]);
            min.z = std::min(min.z, quadVertices[i + 2]);

            max.x = std::max(max.x, quadVertices[i]);
            max.y = std::max(max.y, quadVertices[i + 1]);
            max.z = std::max(max.z, quadVertices[i + 2]);
        }

        // Transformar el AABB al espacio mundial agregando la posici�n del Quad
        box.min = min + m_position; // Sumar la posici�n para obtener las coordenadas en el espacio mundial
        box.max = max + m_position;
    }
    void TileQuad::DrawAABB()
    {
        // Definir los puntos del AABB para dibujar
        float aabbVertices[] = {
            box.min.x, box.min.y, box.min.z,
            box.min.x, box.max.y, box.min.z,
            box.max.x, box.max.y, box.min.z,
            box.max.x, box.min.y, box.min.z,
            box.min.x, box.min.y, box.min.z
        };

        if (lineVAO == 0) 
        {
            glGenVertexArrays(1, &lineVAO);
            glGenBuffers(1, &lineVBO);
        }
        // Enlazar el VAO
        glBindVertexArray(lineVAO);

        // Cargar los datos en el VBO
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(aabbVertices), aabbVertices, GL_STATIC_DRAW);

        // Configurar las propiedades de los v�rtices
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
     

        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(glm::mat4(1.0f), position);

        // Establece la matriz de modelo en tu shader
        libCore::ShaderManager::Get("debug")->use();
        libCore::ShaderManager::Get("debug")->setMat4("model", model);
        glm::vec4 lineColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Color blanco para las l�neas
        libCore::ShaderManager::Get("debug")->setVec4("u_Color", lineColor);
        

        // Dibujar el AABB
        glBindVertexArray(lineVAO);
        glDrawArrays(GL_LINE_STRIP, 0, 5); // Ajusta el conteo seg�n los v�rtices que agregues
        glBindVertexArray(0);
    }
}











//void TileQuad::Draw() {
//
//    if (VAO == 0) {
//        // setup plane VAO
//        glGenVertexArrays(1, &VAO);
//        glGenBuffers(1, &VBO);
//        glBindVertexArray(VAO);
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
//        glBindVertexArray(0); // Unbind VAO
//    }
//
//    // Calcula la transformaci�n para cada quad aqu�
//    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
//    // Aplica escala si es necesario
//    //model = glm::scale(model, glm::vec3(1.0f));
//
//    // Establece la matriz de modelo en tu shader
//    libopengl::ShaderManager::Get("base")->use();
//    libopengl::ShaderManager::Get("base")->setMat4("model", model);
//    libopengl::ShaderManager::Get("base")->setBool("use_texture", false);
//    glm::vec4 myColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
//    libopengl::ShaderManager::Get("base")->setVec4("u_Color", myColor);
//
//    glBindVertexArray(VAO);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//    glBindVertexArray(0);
//
//
//
//
//    UpdateAABB();
//    DrawAABB();
//}









//void TileQuad::UpdateAABB() {
//
//    // Escala para ajustar la posici�n de las AABB
//    glm::vec3 adjustedPosition = position;
//
//    // Asume que el tama�o de cada TileQuad es de 1x1 unidades para este ejemplo
//    float halfWidth = 0.5f * scale; // la mitad del ancho del TileQuad ajustado a la escala
//    float halfHeight = 0.5f * scale; // la mitad del alto del TileQuad ajustado a la escala
//
//    // Calcula los puntos m�nimos y m�ximos de la AABB
//    box.min = (adjustedPosition)-glm::vec3(halfWidth, halfHeight, 0.0f);
//    box.max = (adjustedPosition)+glm::vec3(halfWidth, halfHeight, 0.0f);
//
//    // Actualiza los lineVertices para la AABB
//    GLfloat newLineVertices[] = {
//        // L�neas del plano XY (superior)
//       box.min.x, box.max.y, 0.0f,  box.max.x, box.max.y, 0.0f,
//       box.max.x, box.max.y, 0.0f,  box.max.x, box.min.y, 0.0f,
//       box.max.x, box.min.y, 0.0f,  box.min.x, box.min.y, 0.0f,
//       box.min.x, box.min.y, 0.0f,  box.min.x, box.max.y, 0.0f,
//
//       // L�neas del plano XY (inferior)
//       box.min.x, box.max.y, 0.0f,  box.max.x, box.max.y, 0.0f,
//       box.max.x, box.max.y, 0.0f,  box.max.x, box.min.y, 0.0f,
//       box.max.x, box.min.y, 0.0f,  box.min.x, box.min.y, 0.0f,
//       box.min.x, box.min.y, 0.0f,  box.min.x, box.max.y, 0.0f,
//
//       // L�neas verticales (como si fuera una caja 3D, pero no es necesario si es 2D)
//       box.min.x, box.max.y, 0.0f,  box.min.x, box.max.y, 0.0f,
//       box.max.x, box.max.y, 0.0f,  box.max.x, box.max.y, 0.0f,
//       box.max.x, box.min.y, 0.0f,  box.max.x, box.min.y, 0.0f,
//       box.min.x, box.min.y, 0.0f,  box.min.x, box.min.y, 0.0f,
//    };
//
//
//    // Primero, calcula el punto medio del eje x
//    float punto_medio_x = (box.min.x + box.max.x) / 4.0f;
//
//    GLfloat newLineVerticesEspejo[] = {
//        // L�neas del plano XY (superior e inferior, ya que son id�nticas en este caso)
//        punto_medio_x * 2 - box.min.x, box.max.y, 0.0f,  punto_medio_x * 2 - box.max.x, box.max.y, 0.0f,
//        punto_medio_x * 2 - box.max.x, box.max.y, 0.0f,  punto_medio_x * 2 - box.max.x, box.min.y, 0.0f,
//        punto_medio_x * 2 - box.max.x, box.min.y, 0.0f,  punto_medio_x * 2 - box.min.x, box.min.y, 0.0f,
//        punto_medio_x * 2 - box.min.x, box.min.y, 0.0f,  punto_medio_x * 2 - box.min.x, box.max.y, 0.0f,
//
//        // Repetici�n para las l�neas del plano XY (inferior)
//        punto_medio_x * 2 - box.min.x, box.max.y, 0.0f,  punto_medio_x * 2 - box.max.x, box.max.y, 0.0f,
//        punto_medio_x * 2 - box.max.x, box.max.y, 0.0f,  punto_medio_x * 2 - box.max.x, box.min.y, 0.0f,
//        punto_medio_x * 2 - box.max.x, box.min.y, 0.0f,  punto_medio_x * 2 - box.min.x, box.min.y, 0.0f,
//        punto_medio_x * 2 - box.min.x, box.min.y, 0.0f,  punto_medio_x * 2 - box.min.x, box.max.y, 0.0f,
//
//        // L�neas verticales (sin cambio ya que el espejo no afecta la coordenada y)
//        punto_medio_x * 2 - box.min.x, box.max.y, 0.0f,  punto_medio_x * 2 - box.min.x, box.max.y, 0.0f,
//        punto_medio_x * 2 - box.max.x, box.max.y, 0.0f,  punto_medio_x * 2 - box.max.x, box.max.y, 0.0f,
//        punto_medio_x * 2 - box.max.x, box.min.y, 0.0f,  punto_medio_x * 2 - box.max.x, box.min.y, 0.0f,
//        punto_medio_x * 2 - box.min.x, box.min.y, 0.0f,  punto_medio_x * 2 - box.min.x, box.min.y, 0.0f,
//    };
//
//
//    // Ahora, actualiza box.min.x y box.max.x bas�ndonos en la posici�n espejada
//   // Nota: Esto es un ejemplo conceptual. Deber�s ajustar seg�n la l�gica espec�fica de tu aplicaci�n
//    float newMinX = punto_medio_x * 4 - box.max.x;
//    float newMaxX = punto_medio_x * 4 - box.min.x;
//
//    // Aseg�rate de que newMinX es el m�nimo y newMaxX es el m�ximo
//    box.min.x = std::min(newMinX, newMaxX);
//    box.max.x = std::max(newMinX, newMaxX);
//
//
//    // Copia los nuevos v�rtices a lineVertices
//    memcpy(lineVertices, newLineVerticesEspejo, sizeof(newLineVerticesEspejo));
//
//    // Aseg�rate de actualizar el VBO con los nuevos v�rtices si ya ha sido inicializado
//    if (lineVAO != 0) {
//        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//    }
//}
//
//
//
//void TileQuad::DrawAABB()
//{
//    if (lineVAO == 0) {
//        glGenVertexArrays(1, &lineVAO);
//        glGenBuffers(1, &lineVBO);
//        glBindVertexArray(lineVAO);
//        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//        glEnableVertexAttribArray(0);
//        glBindVertexArray(0); // No es necesario desvincular el VBO aqu�
//    }
//
//    // Calcula la transformaci�n para cada quad aqu�
//    glm::mat4 model = glm::mat4(1.0f);
//    model = glm::translate(glm::mat4(1.0f), position);
//
//    // Establece la matriz de modelo en tu shader
//    libopengl::ShaderManager::Get("debug")->use();
//    libopengl::ShaderManager::Get("debug")->setMat4("model", model);
//    glm::vec4 lineColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Color blanco para las l�neas
//    libopengl::ShaderManager::Get("debug")->setVec4("u_Color", lineColor);
//
//    glBindVertexArray(lineVAO);
//    glDrawArrays(GL_LINES, 0, 24); // Aseg�rate de que el n�mero de v�rtices es correcto
//    glBindVertexArray(0);
//}

