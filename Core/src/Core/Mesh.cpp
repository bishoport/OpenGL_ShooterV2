#include "Mesh.h"

namespace libCore
{
	void Mesh::SetupMesh() {
		VAO.Bind();

		// Genera el VBO y lo enlaza con los v�rtices
		VBO VBO(vertices);

		// Genera el EBO y lo enlaza con los �ndices
		EBO EBO(indices);

		// Habilitar y configurar los atributos de v�rtices
		// Posiciones de v�rtices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// Coordenadas de textura de v�rtices
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texUV));

		// Normales de v�rtices
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// Tangentes de v�rtices
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

		// Bitangentes de v�rtices
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

		// Desvincular para evitar modificaciones accidentales
		glBindVertexArray(0);
		VBO.Unbind();
		EBO.Unbind();
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
			//glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
			glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		}
		else if (drawLike == DRAW_GEOM_LIKE::TRIANGLE)
		{
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}
		

		VAO.Unbind();
	}
}