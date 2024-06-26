#include "Tile.h"
#include <cstring> // Para std::memcpy
#include <tools/ShaderManager.h>
#include <input/InputManager.h>

namespace Game
{
    Tile::Tile()
    {
        // Solo genera VAO y VBO una vez aquí
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // Configurar el VBO y VAO aquí. No necesitas datos aún, solo la estructura.
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW); // 6 vértices, 4 atributos por vértice (x, y, u, v)

        // Posición del atributo de vértice
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Atributo de coordenadas UV
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Tile::SetPosition(float x, float y)
    {
        position = glm::vec3(x , y, 0.0f);
    }

    void Tile::SetupTile()
    {

        float scale = 1.0f; // Factor de escala, por ejemplo, 0.5 para hacer el quad la mitad de grande
        float scaledWidth = tileUVData.w * scale;
        float scaledHeight = tileUVData.h * scale;

        float vertices[] = {  //FLIP HORIZONTAL
            // Primer triángulo
            position.x, position.y, tileUVData.u2, tileUVData.v2,
            position.x, position.y + scaledHeight, tileUVData.u2, tileUVData.v1,
            position.x + scaledWidth, position.y, tileUVData.u1, tileUVData.v2,

            // Segundo triángulo
            position.x + scaledWidth, position.y, tileUVData.u1, tileUVData.v2,
            position.x, position.y + scaledHeight, tileUVData.u2, tileUVData.v1,
            position.x + scaledWidth, position.y + scaledHeight, tileUVData.u1, tileUVData.v1
        };

        //float vertices[] = {
        //    // Primer triángulo
        //    position.x, position.y, tileUVData.u1, tileUVData.v2,                      // Inferior izquierdo
        //    position.x, position.y + tileUVData.h, tileUVData.u1, tileUVData.v1,       // Superior izquierdo
        //    position.x + tileUVData.w, position.y, tileUVData.u2, tileUVData.v2,       // Inferior derecho

        //    // Segundo triángulo
        //    position.x + tileUVData.w, position.y, tileUVData.u2, tileUVData.v2,       // Inferior derecho
        //    position.x, position.y + tileUVData.h, tileUVData.u1, tileUVData.v1,       // Superior izquierdo
        //    position.x + tileUVData.w, position.y + tileUVData.h, tileUVData.u2, tileUVData.v1    // Superior derecho
        //};



        // Actualiza los datos del buffer con las coordenadas de este tile
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    

    void Tile::SetTexture(std::shared_ptr<Texture> newTexture)
    {
        texture = newTexture;
        texture->Bind("");
    }

    void Tile::Draw()
    {
        glBindVertexArray(VAO);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);

        libCore::ShaderManager::Get("base")->use();
        glActiveTexture(GL_TEXTURE0);
        libCore::ShaderManager::Get("base")->setInt("texture1", 0);
        glBindTexture(GL_TEXTURE_2D, texture->ID);
        libCore::ShaderManager::Get("base")->setMat4("model", model);

        // Dibuja el quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Desenlaza el VAO y la textura para limpieza
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}





//
//if (quadVAO == 0)
//{
//    // setup plane VAO
//    glGenVertexArrays(1, &quadVAO);
//    glGenBuffers(1, &quadVBO);
//    glBindVertexArray(quadVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(updatedQuadVertices), updatedQuadVertices, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
//    glBindVertexArray(0); // Unbind VAO
//}
//
//// Asegúrate de actualizar el buffer con las nuevas coordenadas de textura
//glBindVertexArray(quadVAO);
//glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//glBufferData(GL_ARRAY_BUFFER, sizeof(updatedQuadVertices), updatedQuadVertices, GL_STATIC_DRAW);
//
//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//glBindVertexArray(0);