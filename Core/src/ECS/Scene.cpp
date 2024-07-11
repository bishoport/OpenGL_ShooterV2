//#include "Scene.h"
//#include "../Core/EngineOpenGL.h"
//
//
//namespace libCore
//{
//	entt::entity Scene::EntityCreator(Ref<libCore::ModelContainer> modelContainer, std::string nameEntity)
//	{
//		//libCore::EngineOpenGL::GetInstance().AddBounding(modelContainer);
//		
//		unsigned index = entitiesDictionary.size() + 1;
//		entt::entity entity = EntityManager::GetInstance().m_registry.create();
//		modelContainer->entityIdentifier = index;
//		EntityManager::GetInstance().m_registry.emplace<libCore::Transform>(entity);
//		EntityManager::GetInstance().m_registry.emplace<libCore::ColliderComponent>(entity);
//		EntityManager::GetInstance().m_registry.emplace<libCore::MeshRenderer>(entity, modelContainer);
//		if (nameEntity.size() <= 0)
//			nameEntity = "ACTOR_" + std::to_string(index);
//
//		EntityManager::GetInstance().m_registry.emplace<libCore::EntityInfo>(entity, nameEntity);
//
//		modelsInScene.push_back(modelContainer);
//		entitiesDictionary.emplace(index, entity);
//		return entity;
//	}
//
//
//	void Scene::BeginPlay()
//	{
//		//Ref<libCore::ModelContainer> roof = libCore::EngineOpenGL::GetInstance().CreateRoof();
//
//		/*modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreateRoof());
//		modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreateRoof());
//
//		modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreateRoof());*/
//		/*EntityCreator(libCore::EngineOpenGL::GetInstance().CreateRoof());
//		EntityCreator(libCore::EngineOpenGL::GetInstance().CreateRoof());
//		EntityCreator(libCore::EngineOpenGL::GetInstance().CreateRoof());*/
//
//		/*EntityCreator(libCore::EngineOpenGL::GetInstance().CreatePrefabCube(), "Roof 2");
//		EntityCreator(libCore::EngineOpenGL::GetInstance().CreatePrefabCube(), "Roof 3");*/
//		/*Ref<libCore::ModelContainer> modelContainer = libCore::EngineOpenGL::GetInstance().CreateRoof();
//		EntityCreator(modelContainer, "Roof 3");*/
//		//EntityCreator(libCore::EngineOpenGL::GetInstance().CreatePrefabCube(), "Bounding Box");
//		CreateTagsAndLayers();
//		CreateCube();
//		CreateSphere(100, 100, 100);
//	}
//
//
//
//	void Scene::Tick(float DeltaTime)
//	{
//	}
//
//	void Scene::CreateCube()
//	{
//		//EntityCreator(libCore::EngineOpenGL::GetInstance().CreatePrefabCube(glm::vec3(0.0f,0.0f,0.0f)));
//	}
//
//	void Scene::CreateSphere(float radius, unsigned int sectorCount, unsigned int stackCount)
//	{
//		//EntityCreator(libCore::EngineOpenGL::GetInstance().CreatePrefabSphere(radius, sectorCount, stackCount), "");
//	}
//
//	void Scene::CreateTagsAndLayers()
//	{
//		/*std::ifstream file;
//
//		file.open(jsonFile);
//		nlohmann::json jsonData;
//		if (!file.is_open()) {
//			std::cout << "Can not open the file" << std::endl;
//			return;
//		}
//		file >> jsonData;
//		tags = jsonData["tag"].get<std::vector<std::string>>();
//		layers = jsonData["layer"].get<std::vector<std::string>>();*/
//	}
//}