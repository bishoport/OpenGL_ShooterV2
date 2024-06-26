#pragma once
#include <string>
#include <tools/PrimitivesHelper.h>
#include "../Common_Structs.h"
#include "Sprite3D.h"
#include <box2d/include/box2d/box2d.h>
#include "Projectile.h"

namespace Game
{
	class Character : public Sprite3D {
	
	public:
		Character(const CharacterData& data, b2World* world);
		~Character();

		void Update(libCore::Timestep deltaTime) override;
		void Draw() override;

		
		bool m_onGround = true; // Indica si el personaje está tocando el suelo

	private:
		void ChangeState(CharacterState newState);
		void FireProjectile(float angleDegrees);

	private:
		CharacterData m_characterData;

		const float acceleration = 2.5f; // Aceleración progresiva
		const float jumpForce = 6.0f; // Ajusta para controlar la altura del salto
		const float maxSpeed = 2.5f; // Velocidad máxima horizontal

		Animation idleAnimation;
		Animation runAnimation;

		float timeSinceLastFrame = 0.0f;


		CharacterState currentState;
		std::map<CharacterState, Animation> animations;

		bool isMoving = false;
		bool isDashing = false;
		bool lookingToTheLeft = false;
		bool lookingToTheRight = true;

		//--QUAD
		float quadVertices[20] = {
			// Coordenadas de posición      // Coordenadas UV
			-0.5f,  0.5f, 0.0f,             0.0f, 1.0f, // Superior izquierdo
			-0.5f, -0.5f, 0.0f,             0.0f, 0.0f, // Inferior izquierdo
			 0.5f,  0.5f, 0.0f,             1.0f, 1.0f, // Superior derecho
			 0.5f, -0.5f, 0.0f,             1.0f, 0.0f  // Inferior derecho
		};

		
		std::vector<Projectile*> projectiles;

		b2Body*  m_body = nullptr; // Cuerpo Box2D para el personaje
		b2World* m_world;

		std::string CharacterStateToString(CharacterState state) {
			switch (state) {
			case CharacterState::Idle: return "Idle";
			case CharacterState::Run: return "Run";
			case CharacterState::Jump: return "Jump";
			case CharacterState::Attack: return "Attack";
			case CharacterState::Damage: return "Damage";
			case CharacterState::Dead: return "Dead";
			default: return "Unknown State";
			}
		}
	};
}





















//#pragma once
//#include <string>
//#include <PrimitivesHelper.h>
//#include "../Common_Structs.h"
//#include "Sprite3D.h"
//#include <box2d/include/box2d/box2d.h>
//
//namespace Game
//{
//	class Character
//	{
//	public:
//		Character(const CharacterData& data);
//		~Character();
//
//		void Update(libopengl::Timestep deltaTime);
//		void Draw();
//		void DrawRay();
//
//
//		glm::vec3 m_position{ 0.0f, 5.0f, 0.0f }; // También inicializa con (x, y, z)
//		bool m_onGround = true; // Indica si el personaje está tocando el suelo
//		glm::vec3 rayStart = m_position;
//		glm::vec3 rayEnd = glm::vec3(0.0f, 0.0f, 0.0f); // hacia abajo
//
//
//		float m_velocityX = 0.0f; // Velocidad horizontal
//		float m_velocityY = 0.0f; // Velocidad vertical (para el salto)
//
//	private:
//		void ChangeState(CharacterState newState);
//
//
//	private:
//		CharacterData m_characterData;
//
//		const float acceleration = 2.5f; // Aceleración progresiva
//		const float deceleration = 20.4f; // Desaceleración cuando no se presionan teclas
//		const float friction = 0.98f; // Fricción aplicada a la velocidad horizontal
//		const float gravity = 15.8f; // Aumenta la gravedad para caídas más rápidas
//		const float jumpForce = 6.0f; // Ajusta para controlar la altura del salto
//		const float maxSpeed = 2.5f; // Velocidad máxima horizontal
//
//		Animation idleAnimation;
//		Animation runAnimation;
//
//		float timeSinceLastFrame = 0.0f;
//
//
//		CharacterState currentState;
//		std::map<CharacterState, Animation> animations;
//
//		bool isMoving = false;
//		bool lookingToTheLeft = false;
//		bool lookingToTheRight = true;
//
//		glm::mat4 modelToLines = glm::mat4(1.0f);
//
//
//		float quadVertices[20] = {
//			// Coordenadas de posición      // Coordenadas UV
//			-0.5f,  0.5f, 0.0f,             0.0f, 1.0f, // Superior izquierdo
//			-0.5f, -0.5f, 0.0f,             0.0f, 0.0f, // Inferior izquierdo
//			 0.5f,  0.5f, 0.0f,             1.0f, 1.0f, // Superior derecho
//			 0.5f, -0.5f, 0.0f,             1.0f, 0.0f  // Inferior derecho
//		};
//
//
//		GLuint VAO = 0;
//		GLuint VBO = 0;
//		GLuint VAOLine = 0;
//		GLuint VBOLine = 0;
//
//		glm::vec3 rayStartOffset = glm::vec3(0.0f, -0.5f, 0.0f);
//		glm::vec3 rayEndOffset = glm::vec3(0.0f, -0.5f, 0.0f);
//	};
//}

