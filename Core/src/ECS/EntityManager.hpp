//#pragma once
//#include "../tools/ModelLoader.h"
//#include "ECS.h"
//
//
//
//namespace libCore
//{
//	class EntityManager {
//
//	public:
//
//		// Método para obtener la instancia de la clase
//		static EntityManager& GetInstance() {
//			static EntityManager instance;
//			return instance;
//		}
//
//
//		entt::registry m_registry;
//		std::vector<entt::entity> currentEntitiesList;
//
//		void GenerateEntity()
//		{
//			entt::entity entity = m_registry.create();
//
//			m_registry.emplace<libCore::Transform>(entity,
//				glm::vec3(0.0f),
//				glm::vec3(0.0f),
//				glm::vec3(1.0f)
//			);
//		}
//
//		entt::entity* allTheEntities() {
//			//auto view = m_registry.view<>(entt::exclude<>); // Vista de todas las entidades
//			//std::vector<entt::entity> entitesList;
//			//for (auto entity : view) {
//			//	entitesList.push_back(entity);
//			//}
//			//if(entitesList.size() > 0)
//			//	return entitesList.data();
//			std::vector<entt::entity> entitiesList;
//			m_registry.each([&entitiesList](auto entity) {
//				//std::cout << "Value " << entity << std::endl;
//				entitiesList.push_back(entity);
//				});
//			if (currentEntitiesList.size() < entitiesList.size()) {
//
//				for (unsigned i = 0; i < entitiesList.size(); i++) {
//					auto it = std::find(currentEntitiesList.begin(), currentEntitiesList.end(), entitiesList[i]);
//					if (it != nullptr) {
//						currentEntitiesList.emplace_back(entitiesList[i]);
//					}
//				}
//
//				return currentEntitiesList.data();
//			}
//
//			return nullptr;
//		}
//
//	private:
//		EntityManager(); //-->constructor privado
//	};
//
//	// Declaración externa de una referencia a AssetsManager
//	extern EntityManager& entityManager;
//}