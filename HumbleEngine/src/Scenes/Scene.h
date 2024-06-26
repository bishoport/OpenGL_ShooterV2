#pragma once
#include "../ActorSystem/Actor.h"
#include <tools/Camera.h>
#include <tools/PrimitivesHelper.h>
#include <tools/TextureManager.h>
#include "../GameMode.h"
#include "../CollisionsManager.h"
class Scene {
public:

    

    Ref<Texture> diffuse1;
    Ref<Texture> diffuse2;

    Ref<Mesh> testMesh1;
    Ref<Mesh> testMesh2;

    libCore::ShaderManager shaderManager;
    Ref<libCore::Camera> camera;

	void SetUp(Ref<libCore::Camera> newCamera);

	void QuickActor(Ref<Actor> actor, Ref<Mesh> model, Ref<Texture> texture, Ref<libCore::Shader>  shader,
        glm::vec3 position = glm::vec3(0, 0, 0),
        glm::vec3 scale = glm::vec3(1, 1, 1), glm::vec3 rotation = glm::vec3(0), glm::vec3 color = glm::vec3(1, 1, 1),
        bool isColliderAdded = false);


    void Init();

    void Tick(float deltaTime);
};
