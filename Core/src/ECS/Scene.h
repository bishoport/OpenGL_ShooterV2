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

        //--SERIALIZATION
        void SerializeScene(std::string _sceneName , bool saveResource = true);
        void DeserializeScene(std::string _sceneName, bool loadResource = true);

        // Serialización y deserialización solo de componentes (archivo temporal)
        void SerializeComponents();
        void DeserializeComponents();
    };
}
