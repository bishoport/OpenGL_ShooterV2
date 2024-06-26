#pragma once
#include <LibCore.h>
#include <string>


namespace Game
{
	enum class CharacterState {
		Idle,
		Run,
		Jump,
		Attack,
		Damage,
		Dead
	};


	struct Animation
	{
		std::string name = "";
		//unsigned int texture = 0;
		Ref<libCore::Texture> texture;
		int totalFrames = 15; // Cantidad total de frames en tu sprite sheet
		int currentFrame = 0; // El frame actual que quieres mostrar
		float frameTime = 0.1f;
		float frameWidth = 1.0f / totalFrames; // Ancho de cada frame en coordenadas de textura
		bool loop = true; // Por defecto en loop, cambiar según el estado.	
	};



	struct CharacterData
	{
		std::string name;
	};

	
	// Definir una estructura para el hash de std::pair<int, int>
	//struct pair_hash {
	//	template <class T1, class T2>
	//	std::size_t operator () (const std::pair<T1, T2>& pair) const {
	//		auto hash1 = std::hash<T1>{}(pair.first);
	//		auto hash2 = std::hash<T2>{}(pair.second);
	//		return hash1 ^ hash2; // Puedes usar otra función de mezcla si lo prefieres
	//	}
	//};

	struct AABB {
		glm::vec3 min; // Esquina inferior del cuadro de colisión
		glm::vec3 max; // Esquina superior del cuadro de colisión
	};

}