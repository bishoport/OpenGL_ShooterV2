#pragma once
#include "../tools/ModelLoader.h"
#include "../tools/PrimitivesHelper.h"
#include "ECS.h"
#include "../Core/Transform.h"
#include "../Core/Material.h"
#include <entt.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <windows.h>
#include "../Core/EngineOpenGL.h"
#include "Scripts/MyScript.h"
#include "../Timestep.h"
#include "../tools/AssetsManager.h"

namespace libCore
{
    class EntityManager {
    public:
        // Método para obtener la instancia de la clase
        static EntityManager& GetInstance() {
            static EntityManager instance;
            return instance;
        }


        //--ENTITY REGISTRY & COLLECTIONS 
        Ref<entt::registry> m_registry = CreateRef<entt::registry>();
        std::vector<entt::entity> entitiesInRay;
        entt::entity currentSelectedEntityInScene = entt::null;
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------


        //--CREADOR DE ENTITIES
        entt::entity CreateEmptyGameObject(const std::string& name = "")
        {
            entt::entity entity = m_registry->create();
            m_registry->emplace<IDComponent>(entity, UUID());
            auto& tag = m_registry->emplace<TagComponent>(entity);
            tag.Tag = name.empty() ? "Entity" : name;
            m_registry->emplace<TransformComponent>(entity);
            return entity;
        }
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
                    auto& meshComponent = m_registry->emplace<MeshComponent>(entity, mesh, model, true);
                    // Agregar matriz de instancia
                    glm::mat4 instanceMatrix = glm::translate(glm::mat4(1.0f), model->transform->position);
                    meshComponent.instanceMatrices.push_back(instanceMatrix);
                    auto& abbComponent = m_registry->emplace<AABBComponent>(entity);
                    abbComponent.aabb->CalculateAABB(mesh->vertices);
                }
            }

            // Asignar los componentes de herencia
            if (parent != entt::null) {
                AddChild(parent, entity);
            }

            // Recorrer y crear entidades para los hijos del modelo
            for (auto& child : model->children) {
                CreateGameObjectFromModel(child, entity);
            }
        }
        void CreateCubeGameObject(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f))
        {
            // Crear una nueva entidad para el modelo
            entt::entity gameObject = CreateEmptyGameObject("Cube");
            //Transform Component
            auto& transformComponent = GetComponent<TransformComponent>(gameObject);
            transformComponent.transform->position = position;
            //Mesh Component
            auto& meshComponent = AddComponent<MeshComponent>(gameObject);
            meshComponent.mesh = PrimitivesHelper::CreateCube();
            //AABB Component
            auto& abbComponent = m_registry->emplace<AABBComponent>(gameObject);
            abbComponent.aabb->CalculateAABB(meshComponent.mesh->vertices);
            //Material Component
            auto& materialComponent = AddComponent<MaterialComponent>(gameObject);
            materialComponent.material = AssetsManager::GetInstance().getMaterial("default_material");
        }
        void CreateSphereGameObject(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float radius = 1.0f, int sectorCount = 12, int stackCount = 12)
        {
            // Crear una nueva entidad para el modelo
            entt::entity gameObject = CreateEmptyGameObject("Sphere");
            //Transform Component
            auto& transformComponent = GetComponent<TransformComponent>(gameObject);
            transformComponent.transform->position = position;
            //Mesh Component
            auto& meshComponent = AddComponent<MeshComponent>(gameObject);
            meshComponent.mesh = PrimitivesHelper::CreateSphere(radius, sectorCount, stackCount);
            meshComponent.mesh->meshName = "PRIMIVITE_SPHERE";
            //AABB Component
            auto& abbComponent = m_registry->emplace<AABBComponent>(gameObject);
            abbComponent.aabb->CalculateAABB(meshComponent.mesh->vertices);
            //Material Component
            auto& materialComponent = AddComponent<MaterialComponent>(gameObject);
            materialComponent.material = AssetsManager::GetInstance().getMaterial("default_material");
        }
        void CreateCamera()
        {
            // Crear una nueva entidad para el modelo
            entt::entity gameObject = CreateEmptyGameObject("MainCamera");
            //Mesh Component
            auto& cameraComponent = AddComponent<CameraComponent>(gameObject);
        }
        entt::entity GetEntityByName(const std::string& name)
        {
            auto view = m_registry->view<TagComponent>();
            for (auto entity : view) {
                auto& tag = view.get<TagComponent>(entity);
                if (tag.Tag == name) {
                    return entity;
                }
            }
            return entt::null; // Si no se encuentra ninguna entidad con el nombre dado
        }
        void AddChild(entt::entity parent, entt::entity child)
        {
            if (!m_registry->valid(parent) || !m_registry->valid(child)) {
                return;
            }

            // Obtener la transformación global actual del hijo
            auto& childTransformComponent = GetComponent<TransformComponent>(child);
            glm::mat4 childGlobalTransform = childTransformComponent.getGlobalTransform(child, *m_registry);

            // Eliminar el hijo de la lista de hijos del antiguo padre, si lo tiene
            if (HasComponent<ParentComponent>(child)) {
                entt::entity oldParent = GetComponent<ParentComponent>(child).parent;
                if (m_registry->valid(oldParent) && HasComponent<ChildComponent>(oldParent)) {
                    auto& oldParentChildren = GetComponent<ChildComponent>(oldParent).children;
                    oldParentChildren.erase(std::remove(oldParentChildren.begin(), oldParentChildren.end(), child), oldParentChildren.end());
                }
            }

            // Añadir ParentComponent al hijo
            if (!HasComponent<ParentComponent>(child)) {
                AddComponent<ParentComponent>(child).parent = parent;
            }
            else {
                GetComponent<ParentComponent>(child).parent = parent;
            }

            // Añadir ChildComponent al padre
            if (!HasComponent<ChildComponent>(parent)) {
                AddComponent<ChildComponent>(parent).children.push_back(child);
            }
            else {
                auto& parentChildren = GetComponent<ChildComponent>(parent).children;
                if (std::find(parentChildren.begin(), parentChildren.end(), child) == parentChildren.end()) {
                    parentChildren.push_back(child);
                }
            }

            // Actualizar la transformación local del hijo para mantener la transformación global
            childTransformComponent.setTransformFromGlobal(childGlobalTransform, child, *m_registry);
        }
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------


        //--DUPLICATE
        void DuplicateEntity() {
            if (EntityManager::GetInstance().currentSelectedEntityInScene != entt::null) {
                entt::entity selectedEntity = EntityManager::GetInstance().currentSelectedEntityInScene;
                entt::entity parentEntity = entt::null;

                // Verificar si la entidad seleccionada tiene un componente de padre
                if (EntityManager::GetInstance().HasComponent<ParentComponent>(selectedEntity)) {
                    parentEntity = EntityManager::GetInstance().GetComponent<ParentComponent>(selectedEntity).parent;
                }
                entt::entity newEntity = DuplicateEntityRecursively(selectedEntity, parentEntity);
                UpdateAccumulatedTransforms(newEntity);
            }
        }
        entt::entity DuplicateEntityRecursively(entt::entity entity, entt::entity parentEntity) {
            if (!m_registry->valid(entity)) {
                return entt::null;
            }

            // Obtener el nombre original y concatenar con "_clone"
            std::string originalTag = GetComponent<TagComponent>(entity).Tag;
            std::string newTag = originalTag + "_clone";

            // Crear una nueva entidad con el nombre modificado
            entt::entity newEntity = CreateEmptyGameObject(newTag);

            // Copiar componentes
            if (HasComponent<TransformComponent>(entity)) {
                auto& srcTransform = GetComponent<TransformComponent>(entity);
                auto& dstTransform = GetComponent<TransformComponent>(newEntity);

                dstTransform.transform->setMatrix(srcTransform.transform->getMatrix());
            }

            if (HasComponent<MeshComponent>(entity)) {
                auto& srcMesh = GetComponent<MeshComponent>(entity);
                auto& dstMesh = AddComponent<MeshComponent>(newEntity, srcMesh.mesh, srcMesh.originalModel, srcMesh.isInstance);

                // Agregar la matriz de instancia al nuevo MeshComponent
                glm::mat4 instanceMatrix = glm::translate(glm::mat4(1.0f), GetComponent<TransformComponent>(newEntity).transform->position);
                dstMesh.instanceMatrices.push_back(instanceMatrix);
            }

            if (HasComponent<MaterialComponent>(entity)) {
                auto& srcMaterial = GetComponent<MaterialComponent>(entity);
                AddComponent<MaterialComponent>(newEntity, srcMaterial.material);
            }

            if (HasComponent<AABBComponent>(entity)) {
                auto& srcAABB = GetComponent<AABBComponent>(entity);
                auto& dstAABB = AddComponent<AABBComponent>(newEntity);
                auto& srcMesh = GetComponent<MeshComponent>(entity);
                dstAABB.aabb->CalculateAABB(srcMesh.mesh->vertices);
            }

            // Añadir el hijo al padre usando AddChild
            if (parentEntity != entt::null) {
                AddChild(parentEntity, newEntity);
            }

            // Manejar los hijos
            if (HasComponent<ChildComponent>(entity)) {
                auto& srcChild = GetComponent<ChildComponent>(entity);
                for (auto& child : srcChild.children) {
                    DuplicateEntityRecursively(child, newEntity);
                }
            }

            return newEntity;
        }
        void UpdateAccumulatedTransforms(entt::entity entity, const glm::mat4& parentTransform = glm::mat4(1.0f)) {
            if (!m_registry->valid(entity)) {
                return;
            }

            auto& transformComponent = GetComponent<TransformComponent>(entity);

            // Actualizar la transformación acumulada de la entidad actual
            transformComponent.accumulatedTransform = parentTransform * transformComponent.transform->getLocalModelMatrix();

            // Recorrer recursivamente los hijos
            if (HasComponent<ChildComponent>(entity)) {
                auto& childComponent = GetComponent<ChildComponent>(entity);
                for (auto& child : childComponent.children) {
                    UpdateAccumulatedTransforms(child, transformComponent.accumulatedTransform);
                }
            }
        }
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------
        

        //--DESTROY
        void MarkToDeleteRecursively(entt::entity entity)
        {
            if (!m_registry->valid(entity)) {
                return;
            }

            // Marcar la entidad actual
            if (HasComponent<IDComponent>(entity)) {
                GetComponent<IDComponent>(entity).markToDelete = true;
            }

            // Marcar recursivamente los hijos
            if (HasComponent<ChildComponent>(entity)) {
                auto& childComponent = GetComponent<ChildComponent>(entity);
                for (auto& child : childComponent.children) {
                    MarkToDeleteRecursively(child);
                }
            }
        }
        void DestroyEntity(entt::entity entity)
        {
            if (m_registry->valid(entity)) {
                m_registry->destroy(entity);
            }
        }
        void DestroyEntityRecursively(entt::entity entity)
        {
            if (!m_registry->valid(entity)) {
                return;
            }

            // Primero, eliminar todos los hijos recursivamente
            if (HasComponent<ChildComponent>(entity)) {
                auto& childComponent = GetComponent<ChildComponent>(entity);
                for (auto childEntity : childComponent.children) {
                    DestroyEntityRecursively(childEntity);
                }
            }

            // Eliminar referencia del padre si existe
            if (HasComponent<ParentComponent>(entity)) {
                auto& parentComponent = GetComponent<ParentComponent>(entity);
                if (m_registry->valid(parentComponent.parent)) {
                    auto& parentChildren = GetComponent<ChildComponent>(parentComponent.parent).children;
                    parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), entity), parentChildren.end());
                }
            }

            // Luego, eliminar la entidad actual
            DestroyEntity(entity);
        }
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------


        //--DRAW MESH Component
        void DrawGameObjects(const std::string& shader) {
            auto view = m_registry->view<TransformComponent, MeshComponent, MaterialComponent>();
            for (auto entity : view) {
                auto& transform = view.get<TransformComponent>(entity);
                auto& mesh = view.get<MeshComponent>(entity);
                auto& material = view.get<MaterialComponent>(entity);

                DrawOneGameObject(transform, mesh, material, shader);
            }
        }
        void DrawOneGameObject(TransformComponent& transformComponent, MeshComponent& meshComponent, MaterialComponent& materialComponent, const std::string& shader)
        {
            // Valores
            libCore::ShaderManager::Get(shader)->setVec3("albedoColor", materialComponent.material->albedoColor);
            libCore::ShaderManager::Get(shader)->setFloat("normalStrength", materialComponent.material->normalStrength);
            libCore::ShaderManager::Get(shader)->setFloat("metallicValue", materialComponent.material->metallicValue);
            libCore::ShaderManager::Get(shader)->setFloat("roughnessValue", materialComponent.material->roughnessValue);

            // Texturas
            materialComponent.material->albedoMap->Bind(shader);
            materialComponent.material->normalMap->Bind(shader);
            materialComponent.material->metallicMap->Bind(shader);
            materialComponent.material->roughnessMap->Bind(shader);

            // Usar la transformación acumulada
            libCore::ShaderManager::Get(shader)->setMat4("model", transformComponent.accumulatedTransform);

            // Si hay instancias, dibujarlas
            if (!meshComponent.instanceMatrices.empty())
            {
                meshComponent.mesh->DrawInstanced(static_cast<GLsizei>(meshComponent.instanceMatrices.size()), meshComponent.instanceMatrices);
            }

        }
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------


        //--DRAW AABB Component
        void DrawABBGameObjectMeshComponent(const std::string& shader) {
            auto viewAABB = m_registry->view<TransformComponent, AABBComponent>();
            for (auto entity : viewAABB) {
                auto& transformComponent = viewAABB.get<TransformComponent>(entity);
                auto& aabbComponent = viewAABB.get<AABBComponent>(entity);

                if (aabbComponent.aabb->showAABB) {
                    // Obtener la transformación global correcta
                    glm::mat4 globalTransform = transformComponent.getGlobalTransform(entity, *m_registry);

                    libCore::ShaderManager::Get(shader)->setVec4("u_Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                    libCore::ShaderManager::Get(shader)->setMat4("model", globalTransform);

                    aabbComponent.aabb->DrawAABB();
                }
            }
        }
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------


        //--ACTUALIZADOR DE FUNCIONES UPDATES ANTES DEL RENDER DE LOS COMPONENTES
        void UpdateGameObjects(Timestep deltaTime) 
        {
            //Destruccion real antes de actualizar
            auto IDCompView = m_registry->view<IDComponent>();
            for (auto entity : IDCompView) {
                auto& idComponent = GetComponent<IDComponent>(entity);

                if (idComponent.markToDelete == true) {
                    EntityManager::GetInstance().DestroyEntityRecursively(entity);
                }
            }

            // Actualizar scripts (si es necesario)
            if (EngineOpenGL::GetInstance().engineState == EDITOR_PLAY || EngineOpenGL::GetInstance().engineState == PLAY) {
                auto scriptView = m_registry->view<ScriptComponent>();
                for (auto entity : scriptView) {
                    auto& scriptComponent = scriptView.get<ScriptComponent>(entity);
                    scriptComponent.Update(deltaTime.GetMilliseconds());
                }
            }

            //UPDATE ALL TRANSFORM CHILDREN
            auto rootView = m_registry->view<TransformComponent>(entt::exclude<ParentComponent>);
            for (auto entity : rootView) {
                UpdateAccumulatedTransforms(entity);
            }


            // Actualizar las cámaras
            auto CameraCompView = m_registry->view<CameraComponent, TransformComponent>();
            for (auto entity : CameraCompView) {
                auto& cameraComponent = GetComponent<CameraComponent>(entity);
                auto& transformComponent = GetComponent<TransformComponent>(entity);

                glm::vec3 forward = transformComponent.transform->rotation * glm::vec3(0.0f, 0.0f, -1.0f);
                cameraComponent.view = glm::lookAt(transformComponent.transform->position, transformComponent.transform->position + forward, cameraComponent.Up);

                if (cameraComponent.width > 0 && cameraComponent.height > 0) {
                    cameraComponent.projection = glm::perspective(glm::radians(cameraComponent.FOVdeg), (float)cameraComponent.width / (float)cameraComponent.height, cameraComponent.nearPlane, cameraComponent.farPlane);
                }

                cameraComponent.cameraMatrix = cameraComponent.projection * cameraComponent.view;
            }


            // Actualizar los AABB
            auto viewAABB = m_registry->view<TransformComponent, AABBComponent>();
            for (auto entity : viewAABB) {
                auto& transformComponent = viewAABB.get<TransformComponent>(entity);
                auto& aabbComponent = viewAABB.get<AABBComponent>(entity);

                glm::mat4 globalTransform = transformComponent.accumulatedTransform;
                aabbComponent.aabb->UpdateAABB(globalTransform);
            }
        }
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------


        //--AABB Component MOUSE CHECKER
        void CheckRayModelIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection)
        {
            auto viewAABB = m_registry->view<TransformComponent, AABBComponent>();
            for (auto entity : viewAABB) {
                auto& transformComponent = viewAABB.get<TransformComponent>(entity);
                auto& aabbComponent = viewAABB.get<AABBComponent>(entity);
                // Obtener la transformación global correcta
                glm::mat4 globalTransform = transformComponent.getGlobalTransform(entity, *m_registry);
                // Transformar AABB
                glm::vec3 transformedMin = glm::vec3(globalTransform * glm::vec4(aabbComponent.aabb->minBounds, 1.0));
                glm::vec3 transformedMax = glm::vec3(globalTransform * glm::vec4(aabbComponent.aabb->maxBounds, 1.0));
                // Verificar la intersección del rayo con la AABB transformada
                if (rayIntersectsBoundingBox(rayOrigin, rayDirection, transformedMin, transformedMax)) {
                    entitiesInRay.push_back(entity);
                }
            }
        }
        bool rayIntersectsBoundingBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3 boxMin, glm::vec3 boxMax)
        {
            float tMin = (boxMin.x - rayOrigin.x) / rayDirection.x;
            float tMax = (boxMax.x - rayOrigin.x) / rayDirection.x;
            if (tMin > tMax) std::swap(tMin, tMax);
            float tyMin = (boxMin.y - rayOrigin.y) / rayDirection.y;
            float tyMax = (boxMax.y - rayOrigin.y) / rayDirection.y;
            if (tyMin > tyMax) std::swap(tyMin, tyMax);
            if ((tMin > tyMax) || (tyMin > tMax)) return false;
            if (tyMin > tMin) tMin = tyMin;
            if (tyMax < tMax) tMax = tyMax;
            float tzMin = (boxMin.z - rayOrigin.z) / rayDirection.z;
            float tzMax = (boxMax.z - rayOrigin.z) / rayDirection.z;
            if (tzMin > tzMax) std::swap(tzMin, tzMax);
            if ((tMin > tzMax) || (tzMin > tMax)) return false;
            if (tzMin > tMin) tMin = tzMin;
            if (tzMax < tMax) tMax = tzMax;
            return true;
        }
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------


        //--COMPONENTS HANDLERS
        template<typename T>
        T& GetComponent(entt::entity entity) {
            return m_registry->get<T>(entity);
        }

        template<typename T>
        bool HasComponent(entt::entity entity) {
            return m_registry->has<T>(entity);
        }

        template<typename T, typename... Args>
        T& AddComponent(entt::entity entity, Args&&... args) {
            return m_registry->emplace<T>(entity, std::forward<Args>(args)...);
        }

        // Método plantilla para agregar un componente con script
        template<typename ScriptType>
        ScriptComponent& AddComponentWithScript(entt::entity entity, const std::string& scriptName) {
            auto& scriptComponent = m_registry->emplace<ScriptComponent>(entity);
            scriptComponent.instance = ScriptFactory::GetInstance().CreateScript(scriptName);
            scriptComponent.instance->SetEntity(entity, m_registry);
            return scriptComponent;
        }

        template<typename T>
        void RemoveComponent(entt::entity entity) {
            m_registry->remove<T>(entity);
        }
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------

    private:
        //-Constructor privado para el patrón Singleton
        EntityManager() {
            RegisterScripts();
        }

        //-Registra otros scripts aquí
        void RegisterScripts()
        {
            ScriptFactory::GetInstance().RegisterScript<MyScript>("MyScript");
        }
    };
}








//Ref<Model> CreatePrefabDot(const glm::vec3& pos, const glm::vec3& polygonColor)
//{
//	auto modelBuild = CreateRef<Model>();
//	modelBuild->transform->position = pos;
//	modelBuild->name = "PRIMITIVE_DOT";
//	modelBuild->meshes.push_back(PrimitivesHelper::CreateDot());
//
//
//	//--DEFAULT_MATERIAL
//	auto material = CreateRef<Material>();
//	material->materialName = "default_material";
//
//	material->albedoColor = polygonColor;
//
//	material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
//
//	modelBuild->materials.push_back(material);
//
//	return modelBuild;
//}

//Ref<Model> CreatePrefabLine(const glm::vec3& point1, const glm::vec3& point2)
//{
//	auto modelBuild = CreateRef<Model>();
//
//	modelBuild->name = "PRIMITIVE_LINE";
//	modelBuild->meshes.push_back(PrimitivesHelper::CreateLine(point1, point2));
//
//
//	//--DEFAULT_MATERIAL
//	auto material = CreateRef<Material>();
//	material->materialName = "default_material";
//
//	material->albedoColor.r = 1.0f;
//	material->albedoColor.g = 1.0f;
//	material->albedoColor.b = 1.0f;
//
//	material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
//	material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
//	material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
//	material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
//	material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");
//
//	modelBuild->materials.push_back(material);
//
//	return modelBuild;
//}
//Ref<Model> CreateTriangle(const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3)
//{
//	auto modelBuild = CreateRef<Model>();
//
//
//	modelBuild->name = "PRIMIVITE_TRIANGLE";
//	modelBuild->meshes.push_back(PrimitivesHelper::CreateTriangle(pos1, pos2, pos3));
//
//
//	//--DEFAULT_MATERIAL
//	auto material = CreateRef<Material>();
//	material->materialName = "default_material";
//
//	material->albedoColor.r = 1.0f;
//	material->albedoColor.g = 1.0f;
//	material->albedoColor.b = 1.0f;
//
//	material->albedoMap = AssetsManager::GetInstance().GetTexture("checker");
//	material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
//	material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
//	material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
//	material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");
//
//	modelBuild->materials.push_back(material);
//
//	return modelBuild;
//}
//Ref<Model> CreatePrefabSphere(float radius, unsigned int sectorCount, unsigned int stackCount)
//{
//	auto modelBuild = CreateRef<Model>();
//
//	modelBuild->name = "PRIMIVITE_SPHERE";
//	modelBuild->meshes.push_back(PrimitivesHelper::CreateSphere(0.01f, 6, 6));
//
//	//--DEFAULT_MATERIAL
//	auto material = CreateRef<Material>();
//	material->materialName = "default_material";
//
//	material->albedoColor.r = 1.0f;
//	material->albedoColor.g = 1.0f;
//	material->albedoColor.b = 1.0f;
//
//	material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
//	material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
//	material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
//	material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
//	material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");
//
//	modelBuild->materials.push_back(material);
//
//	return modelBuild;
//}
//Ref<Model> CreatePrefabCube(glm::vec3 position)
//{
//	auto modelBuild = CreateRef<Model>();
//
//	modelBuild->name = "PRIMIVITE_CUBE";
//	modelBuild->meshes.push_back(PrimitivesHelper::CreateCube());
//	modelBuild->transform->position = position;
//
//	//--DEFAULT_MATERIAL
//	auto material = CreateRef<Material>();
//	material->materialName = "default_material";
//
//	material->albedoColor.r = 1.0f;
//	material->albedoColor.g = 1.0f;
//	material->albedoColor.b = 1.0f;
//
//	material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
//	material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
//	material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
//	material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
//	material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");
//
//	modelBuild->materials.push_back(material);
//
//	return modelBuild;
//}