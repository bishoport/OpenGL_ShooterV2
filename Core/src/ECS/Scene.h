#pragma once

#include "../LibCoreHeaders.h"
#include "../Timestep.h"


namespace libCore {

    class Scene {

    public:

        std::string sceneName = "new_scene";

        Scene();
        static Scene& GetInstance() {
            static Scene instance;
            return instance;
        }

        //--SCENE LIFE-CYCLE
        void Init();
        void Update(Timestep m_deltaTime);
        void Render(Timestep m_deltaTime);
        void SerializeScene();
        void DeserializeScene();

    private:
    };
}
