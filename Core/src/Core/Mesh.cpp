#include "Mesh.h"
#include <limits>

namespace libCore
{
    void Mesh::SetupMesh() {
        VAO.Bind();

        // Genera el VBO y lo enlaza con los vértices
        VBO VBO(vertices);

        // Genera el EBO y lo enlaza con los índices
        EBO EBO(indices);

        // Habilitar y configurar los atributos de vértices
        // Posiciones de vértices
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // Coordenadas de textura de vértices
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texUV));

        // Normales de vértices
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        // Tangentes de vértices
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

        // Bitangentes de vértices
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

        // Desvincular para evitar modificaciones accidentales
        glBindVertexArray(0);
        VBO.Unbind();
        EBO.Unbind();

        // Calculate the AABB after setting up the mesh
        CalculateAABB();
        PrepareAABB();
    }

    void Mesh::Draw()
    {
        VAO.Bind();

        if (drawLike == DRAW_GEOM_LIKE::DOT)
        {
            glPointSize(5.0f);
            glDrawArrays(GL_POINTS, 0, 1);
        }
        else if (drawLike == DRAW_GEOM_LIKE::LINE)
        {
            glLineWidth(1.0f);
            glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
        }
        else if (drawLike == DRAW_GEOM_LIKE::TRIANGLE)
        {
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }

        VAO.Unbind();
    }

    

    void Mesh::CalculateAABB() {
        // Inicializar límites min y max
        minBounds = glm::vec3(std::numeric_limits<float>::max());
        maxBounds = glm::vec3(std::numeric_limits<float>::lowest());

        for (const auto& vertex : vertices) {
            minBounds = glm::min(minBounds, vertex.position);
            maxBounds = glm::max(maxBounds, vertex.position);
        }
    }


    void Mesh::PrepareAABB() {
        //-----------------------------------PREPARE BOUNDING BOX LOCAL-----------------------------------
        glCreateVertexArrays(1, &VAO_BB);
        glCreateBuffers(1, &VBO_BB);
        glCreateBuffers(1, &EBO_BB);  // Crear EBO

        glVertexArrayVertexBuffer(VAO_BB, 0, VBO_BB, 0, 3 * sizeof(GLfloat));

        glm::vec3 min = minBounds;
        glm::vec3 max = maxBounds;

        glm::vec3 vertices[8] = {
            {min.x, min.y, min.z}, {max.x, min.y, min.z},
            {max.x, max.y, min.z}, {min.x, max.y, min.z},
            {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {max.x, max.y, max.z}, {min.x, max.y, max.z}
        };

        glNamedBufferStorage(VBO_BB, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);  // Llenar VBO

        // Definir índices para las líneas de la Bounding Box
        GLuint indices[24] = {
            0, 1, 1, 2, 2, 3, 3, 0,  // base inferior
            4, 5, 5, 6, 6, 7, 7, 4,  // base superior
            0, 4, 1, 5, 2, 6, 3, 7   // aristas laterales
        };

        glNamedBufferStorage(EBO_BB, sizeof(indices), indices, GL_DYNAMIC_STORAGE_BIT);  // Llenar EBO

        // Asociar EBO con VAO
        glVertexArrayElementBuffer(VAO_BB, EBO_BB);

        glEnableVertexArrayAttrib(VAO_BB, 0);
        glVertexArrayAttribFormat(VAO_BB, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(VAO_BB, 0, 0);
        //-------------------------------------------------------------------------------------------------
    }

    void Mesh::UpdateAABB(const glm::mat4& modelMatrix) {
        glm::vec3 localVertices[8] = {
            {minBounds.x, minBounds.y, minBounds.z}, {maxBounds.x, minBounds.y, minBounds.z},
            {maxBounds.x, maxBounds.y, minBounds.z}, {minBounds.x, maxBounds.y, minBounds.z},
            {minBounds.x, minBounds.y, maxBounds.z}, {maxBounds.x, minBounds.y, maxBounds.z},
            {maxBounds.x, maxBounds.y, maxBounds.z}, {minBounds.x, maxBounds.y, maxBounds.z}
        };

        glm::vec3 transformedVertices[8];
        for (int i = 0; i < 8; ++i) {
            glm::vec4 transformedVertex = modelMatrix * glm::vec4(localVertices[i], 1.0f);
            transformedVertices[i] = glm::vec3(transformedVertex);
        }

        glm::vec3 newMin = transformedVertices[0];
        glm::vec3 newMax = transformedVertices[0];
        for (int i = 1; i < 8; ++i) {
            newMin = glm::min(newMin, transformedVertices[i]);
            newMax = glm::max(newMax, transformedVertices[i]);
        }

        worldMinBounds = newMin;
        worldMaxBounds = newMax;
    }


    void Mesh::DrawAABB() {
        // Dibuja la Bounding Box
        glLineWidth(5);
        glBindVertexArray(VAO_BB);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }


    bool Mesh::IsPointInsideAABB(const glm::vec3& point) {
        return (point.x >= minBounds.x && point.x <= maxBounds.x) &&
            (point.y >= minBounds.y && point.y <= maxBounds.y) &&
            (point.z >= minBounds.z && point.z <= maxBounds.z);
    }

}
