#pragma once
#include "../LibCoreHeaders.h"
#include<string>

#include"VAO.h"
#include"EBO.h"
#include"Texture.h"

namespace libCore
{
	class Mesh
	{
	public:
		// Initializes the mesh
		Mesh() = default;

		std::string meshName = "unknow_mesh_name";
		glm::vec3 meshLocalPosition = glm::vec3(0.0f);

		std::vector <Vertex>  vertices;
		std::vector <GLuint>  indices;

		DRAW_GEOM_LIKE drawLike = DRAW_GEOM_LIKE::TRIANGLE;

		void SetupMesh();

		void Draw();

	private:
		VAO VAO;
	};
}