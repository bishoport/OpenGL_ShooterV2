#include "Mesh.h"

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


//#include "Mesh.h"
//
//namespace libCore
//{
//	void Mesh::SetupMesh() {
//		VAO.Bind();
//
//		// Genera el VBO y lo enlaza con los vértices
//		VBO VBO(vertices);
//
//		// Genera el EBO y lo enlaza con los índices
//		EBO EBO(indices);
//
//		// Habilitar y configurar los atributos de vértices
//		// Posiciones de vértices
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
//
//		// Coordenadas de textura de vértices
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texUV));
//
//		// Normales de vértices
//		glEnableVertexAttribArray(2);
//		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
//
//		// Tangentes de vértices
//		glEnableVertexAttribArray(3);
//		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
//
//		// Bitangentes de vértices
//		glEnableVertexAttribArray(4);
//		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
//
//		// Desvincular para evitar modificaciones accidentales
//		glBindVertexArray(0);
//		VBO.Unbind();
//		EBO.Unbind();
//	}
//
//	void Mesh::Draw()
//	{
//		VAO.Bind();
//
//		if (drawLike == DRAW_GEOM_LIKE::DOT)
//		{
//			glPointSize(5.0f);
//			glDrawArrays(GL_POINTS, 0, 1);
//		}
//		else if (drawLike == DRAW_GEOM_LIKE::LINE)
//		{
//			glLineWidth(1.0f);
//			//glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
//			glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
//		}
//		else if (drawLike == DRAW_GEOM_LIKE::TRIANGLE)
//		{
//			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//		}
//		
//
//		VAO.Unbind();
//	}
//}






// IDs de huesos
//glEnableVertexAttribArray(5);
//glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
//
//// Pesos de huesos
//glEnableVertexAttribArray(6);
//glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));



	//void Mesh::SetupMesh()
	//{

	//	VAO.Bind();
	//	// Generates Vertex Buffer Object and links it to vertices
	//	VBO VBO(vertices);
	//	// Generates Element Buffer Object and links it to indices
	//	EBO EBO(indices);


	//	//Links VBO attributes such as coordinates and colors to VAO
	//	//VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	//	//VAO.LinkAttrib(VBO, 1, 2, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	//	//VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(5 * sizeof(float)));


	//	//VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
	//	//VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	//	//VAO.LinkAttrib(VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texUV));
	//	
	//	// set the vertex attribute pointers

	//	// vertex Positions
	//	glEnableVertexAttribArray(0);
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//	//vertex normals
	//	glEnableVertexAttribArray(1);
	//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	//	//vertex texture coords
	//	glEnableVertexAttribArray(2);
	//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texUV));

	//	//vertex tangent
	//	glEnableVertexAttribArray(3);
	//	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	//	//vertex bitangent
	//	glEnableVertexAttribArray(4);
	//	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	//	//ids
	//	glEnableVertexAttribArray(5);
	//	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

	//	//weights
	//	glEnableVertexAttribArray(6);
	//	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
	//	glBindVertexArray(0);


	//	//Unbind all to prevent accidentally modifying them
	//	VAO.Unbind();
	//	VBO.Unbind();
	//	EBO.Unbind();
	//}

	//void Mesh::SetupSkeletalMesh()
	//{
	//	VAO.Bind();
	//	// Generates Vertex Buffer Object and links it to vertices
	//	VBO VBO(vertices);
	//	// Generates Element Buffer Object and links it to indices
	//	EBO EBO(indices);

	//	// set the vertex attribute pointers
	//	// vertex Positions
	//	glEnableVertexAttribArray(0);
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	//	// vertex normals
	//	glEnableVertexAttribArray(1);
	//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	//	// vertex texture coords
	//	glEnableVertexAttribArray(2);
	//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texUV));

	//	// vertex tangent
	//	glEnableVertexAttribArray(3);
	//	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	//	// vertex bitangent
	//	glEnableVertexAttribArray(4);
	//	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
	//	// ids
	//	glEnableVertexAttribArray(5);
	//	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

	//	// weights
	//	glEnableVertexAttribArray(6);
	//	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
	//	glBindVertexArray(0);

	//	// Unbind all to prevent accidentally modifying them
	//	VAO.Unbind();
	//	VBO.Unbind();
	//	EBO.Unbind();
	//}