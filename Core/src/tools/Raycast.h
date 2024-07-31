//#pragma once
//#include "Camera.h"
//#include "../ECS/EntityManager.hpp"
//class Raycast {
//
//public:
//	void ProjectRay(libCore::Camera camera);
//
//	void MakePing();
//	bool rayCastIntersecting(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3 boxMin, glm::vec3 boxMax);
//	bool isValidPair(const std::pair<double, double>& p) {
//		return !std::isnan(p.first) && !std::isnan(p.second);
//	}
//};





void CreateGameObjectFromModel(Ref<Model> model, entt::entity parent)
{
    // Crear una nueva entidad para el modelo
    entt::entity entity = CreateEmptyGameObject(model->name);

    // Crear un nuevo TransformComponent para la entidad
    auto& transformComponent = GetComponent<TransformComponent>(entity);
    transformComponent.transform->position = model->transform->position;

    // Asignar los componentes de MaterialComponent si el modelo tiene materiales
    if (!model->materials.empty()) {
        for (auto& material : model->materials) {
            m_registry->emplace<MaterialComponent>(entity, material);
        }
    }

    // Asignar los componentes de MeshComponent si el modelo tiene mallas
    if (!model->meshes.empty()) {
        for (auto& mesh : model->meshes) {
            auto& meshComponent = m_registry->emplace<MeshComponent>(entity, mesh);

            // Agregar matriz de instancia
            glm::mat4 instanceMatrix = glm::translate(glm::mat4(1.0f), model->transform->position);
            meshComponent.instanceMatrices.push_back(instanceMatrix);

            auto& abbComponent = m_registry->emplace<AABBComponent>(entity);
            abbComponent.aabb->CalculateAABB(mesh->vertices);
        }
    }

    // Asignar los componentes de herencia
    if (parent != entt::null) {
        m_registry->emplace<ParentComponent>(entity, parent);
        auto& parentComponent = m_registry->get_or_emplace<ChildComponent>(parent);
        parentComponent.children.push_back(entity);
    }

    // Recorrer y crear entidades para los hijos del modelo
    for (auto& child : model->children) {
        CreateGameObjectFromModel(child, entity);
    }
}
