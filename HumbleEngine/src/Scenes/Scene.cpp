#include "Scene.h"
#include "../ActorSystem/Components/RendererComponent.h"

void Scene::SetUp(Ref<libCore::Camera> newCamera)
{
	camera = newCamera;

	testMesh1 = libCore::PrimitivesHelper::CreateCube();
	testMesh2 = libCore::PrimitivesHelper::CreateSphere(1.0, 20, 20);

	//-- PREPARE TEXTURES
	std::string texturesDirectory = "C:/Users/migue/Desktop/Projects/OpenGLSandbox/HumbleEngine/assets/textures/";
	diffuse1 = libCore::TextureManager::LoadTexture((texturesDirectory + "floor_1.jpg").c_str(), "diffuse", 0);
	diffuse2 = libCore::TextureManager::LoadTexture((texturesDirectory + "floor_2.jpg").c_str(), "diffuse", 0);


	std::string shadersDirectory = "C:/Users/migue/Desktop/Projects/OpenGLSandbox/HumbleEngine/assets/shaders/";
	shaderManager.setShaderDataLoad("basic", shadersDirectory + "basic.vert", shadersDirectory + "basic.frag");
	shaderManager.setShaderDataLoad("text", shadersDirectory + "text.vert", shadersDirectory + "text.frag");
	shaderManager.LoadAllShaders();

	Ref<Actor> cubeActor = std::make_shared<Actor>("Cube");
	Ref<libCore::Shader> basicShader = Ref<libCore::Shader>(shaderManager.Get("basic"));
	QuickActor(cubeActor, testMesh1, diffuse1, basicShader, glm::vec3(0, 0, 0), glm::vec3(1.f), glm::vec3(0), glm::vec3(1, 1, 1), true);

	Ref<Actor> cubeActor2 = std::make_shared<Actor>("Cube 2");

	QuickActor(cubeActor2, testMesh1, diffuse1, basicShader, glm::vec3(2, 0,0), glm::vec3(.5f), glm::vec3(0), glm::vec3(1,1,1), true);

	//-----------------------------------------------------------------
}

void Scene::QuickActor(Ref<Actor> actor, Ref<Mesh> model, Ref<Texture> texture, Ref<libCore::Shader> shader,
	glm::vec3 position, glm::vec3 scale, 
	glm::vec3 rotation , 
	glm::vec3 color, bool isColliderAdded)
{
	Ref<RendererComponent> newRenderer = std::make_shared<RendererComponent>(actor->GetComponent<TransformComponent>());
	actor->AddComponent(newRenderer);
	actor->ListComponents();
	actor->GetComponent<RendererComponent>()->SetModelInfo(model, shader, camera, texture);
	actor->GetComponent<TransformComponent>()->InitialConfiguration(position, rotation, scale);
	if (isColliderAdded) {
		actor->AddComponent(std::make_shared<ColliderComponent>());
	//	actor->GetComponent<ColliderComponent>()->ReceiveBounds();
		CollisionsManager::GetInstance().AddCollider(actor);


	}
	//actor->GetComponent<RendererComponent>().SetModelInfo(model, shader, nullptr);
	GameMode::GetInstance().scenesActors.emplace_back(actor);	
}

void Scene::Init()
{
	CollisionsManager::GetInstance().Initialize();
	GameMode::GetInstance().BeginPlay();
}

void Scene::Tick(float deltaTime)
{
	GameMode::GetInstance().PreRenderer();
	GameMode::GetInstance().PostRenderer();
	CollisionsManager::GetInstance().Execute();
	GameMode::GetInstance().Tick(deltaTime);
}


