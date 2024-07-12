#pragma once

#include "../LibCoreHeaders.h"
#include "../Timestep.h"
#include "entt.hpp"



namespace libCore
{
	class Scene {

	public:



	public:
		// Método para obtener la instancia de la clase
		static Scene& GetInstance() {
			static Scene instance;
			return instance;
		}


		//--SCENE LIFE-CYCLE
		void Init();
		void Update(Timestep m_deltaTime);
		void Render(Timestep m_deltaTime);

	private:




	private:
		Scene();
	};

}
