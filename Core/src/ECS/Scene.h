#pragma once

#include "../LibCoreHeaders.h"
#include "../Timestep.h"



namespace libCore
{

	

	class Scene {

	public:
		Scene();


	public:

		

		// M�todo para obtener la instancia de la clase
		static Scene& GetInstance() {
			static Scene instance;
			return instance;
		}

		//--SCENE LIFE-CYCLE
		void Init();
		void Update(Timestep m_deltaTime);
		void Render(Timestep m_deltaTime);
		//-----------------------------------


	private:


	private:
		
	};

}
