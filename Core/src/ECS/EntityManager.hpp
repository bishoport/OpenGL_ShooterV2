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

#include "Scripts/MyScript.h"
#include "../Timestep.h"

namespace libCore 
{


    /*class Entity {

    public:

        Entity() = default;

        Entity(entt::entity handle): m_EntityHandle(handle) {}

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args);

        template<typename T>
        T& GetComponent();

        template<typename T>
        bool HasComponent() const;

        template<typename T>
        void RemoveComponent();

        operator bool() const { return m_EntityHandle != entt::null; }
        operator entt::entity() const { return m_EntityHandle; }
        operator uint32_t() const { return (uint32_t)m_EntityHandle; }

        entt::entity GetHandle() const { return m_EntityHandle; }

    private:
        entt::entity m_EntityHandle{ entt::null };
    };*/





    class EntityManager {
    public:

        // Método para obtener la instancia de la clase
        static EntityManager& GetInstance() {
            static EntityManager instance;
            return instance;
        }

        //--ENTITY COLLECTIONS
        Ref<entt::registry> m_registry = CreateRef<entt::registry>();
        std::vector<entt::entity> entitiesInRay;
        entt::entity currentSelectedEntityInScene = entt::null;



        //--CREADOR DE ENTITIES
        entt::entity CreateEmptyGameObject()
        {
            entt::entity entity = m_registry->create();
            // Asignar el componente TransformComponent
            auto& transformComponent = m_registry->emplace<TransformComponent>(entity);
            return entity;
        }
        
        void CreateExternalModelGameObject(ImportModelData importModelData) {
            Ref<Model> model = ModelLoader::LoadModel(importModelData);
            CreateGameObjectFromModel(model, entt::null);
            model.reset();
        }

        void CreateCubeGameObject(glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f))
        {
                // Crear una nueva entidad para el modelo
                entt::entity gameObject = CreateEmptyGameObject();
                
                //Transform Component
                auto& transformComponent = GetComponent<TransformComponent>(gameObject);
                transformComponent.transform->position = position;

                //Mesh Component
                auto& meshComponent = AddComponent<MeshComponent>(gameObject);
                meshComponent.mesh = PrimitivesHelper::CreateCube();
                meshComponent.mesh->meshName = "PRIMIVITE_CUBE";

            	//Material Component
                auto& materialComponent = AddComponent<MaterialComponent>(gameObject);
                materialComponent.material->materialName = "default_material";
            	materialComponent.material->albedoColor.r = 1.0f;
            	materialComponent.material->albedoColor.g = 1.0f;
            	materialComponent.material->albedoColor.b = 1.0f;
            	materialComponent.material->albedoMap    = AssetsManager::GetInstance().GetTexture("default_albedo");
            	materialComponent.material->normalMap    = AssetsManager::GetInstance().GetTexture("default_normal");
            	materialComponent.material->metallicMap  = AssetsManager::GetInstance().GetTexture("default_metallic");
            	materialComponent.material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
            	materialComponent.material->aOMap        = AssetsManager::GetInstance().GetTexture("default_ao");
        }

        void CreateSphereGameObject(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float radius = 0.01f, int sectorCount = 6, int stackCount = 6)
        {
            // Crear una nueva entidad para el modelo
            entt::entity gameObject = CreateEmptyGameObject();

            //Transform Component
            auto& transformComponent = GetComponent<TransformComponent>(gameObject);
            transformComponent.transform->position = position;

            //Mesh Component
            auto& meshComponent = AddComponent<MeshComponent>(gameObject);
            meshComponent.mesh = PrimitivesHelper::CreateSphere(radius, sectorCount, stackCount);
            meshComponent.mesh->meshName = "PRIMIVITE_SPHERE";

            //Material Component
            auto& materialComponent = AddComponent<MaterialComponent>(gameObject);
            materialComponent.material->materialName = "default_material";
            materialComponent.material->albedoColor.r = 1.0f;
            materialComponent.material->albedoColor.g = 1.0f;
            materialComponent.material->albedoColor.b = 1.0f;
            materialComponent.material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
            materialComponent.material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
            materialComponent.material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
            materialComponent.material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
            materialComponent.material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");
        }
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------








        //--PINTADOR DE ENTITIES
        void DrawGameObjects(const std::string& shader) {
            auto view = m_registry->view<TransformComponent, MeshComponent, MaterialComponent>();
            for (auto entity : view) {
                auto& transform = view.get<TransformComponent>(entity);
                auto& mesh = view.get<MeshComponent>(entity);
                auto& material = view.get<MaterialComponent>(entity);

                // Aquí llamamos a la función de dibujo de tu motor gráfico
                DrawOneGameObject(transform, mesh, material, shader);
            }
        }
        void DrawABBGameObjectMeshComponent(const std::string& shader) {
            auto view = m_registry->view<TransformComponent, MeshComponent>();
            for (auto entity : view) {
                auto& transform = view.get<TransformComponent>(entity);
                auto& mesh = view.get<MeshComponent>(entity);

                if (mesh.mesh->showAABB == true)
                {
                    libCore::ShaderManager::Get(shader)->setVec4("u_Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                    libCore::ShaderManager::Get(shader)->setMat4("model", transform.transform->getMatrix());

                    mesh.mesh->DrawAABB();
                } 
            }
        }
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------


        //--ACTUALIZADOR DE FUNCIONES UPDATES ANTES DEL RENDER DE LOS COMPONENTES
        void UpdateGameObjects(Timestep deltaTime) {

            // Inicializar y actualizar scripts
            auto scriptView = m_registry->view<ScriptComponent>();
            for (auto entity : scriptView) {
                auto& scriptComponent = scriptView.get<ScriptComponent>(entity);
                scriptComponent.Update(deltaTime.GetMilliseconds());
            }


            // Crear una vista para las entidades con los componentes TransformComponent
            auto trasnformView = m_registry->view<TransformComponent>();

            // Recorrer todas las entidades y actualizar sus transformaciones acumuladas
            for (auto entity : trasnformView) {
                auto& transformComponent = trasnformView.get<TransformComponent>(entity);

                if (HasComponent<ParentComponent>(entity)) {
                    auto& parentComponent = GetComponent<ParentComponent>(entity);
                    auto& parentTransformComponent = GetComponent<TransformComponent>(parentComponent.parent);

                    transformComponent.accumulatedTransform = parentTransformComponent.accumulatedTransform * transformComponent.transform->getLocalModelMatrix();
                }
                else {
                    transformComponent.accumulatedTransform = transformComponent.transform->getLocalModelMatrix();
                }

                transformComponent.transform->updateRotationFromEulerAngles();
            }

            // Actualizar AABBs después de que todas las transformaciones acumuladas estén calculadas
            auto meshView = m_registry->view<TransformComponent, MeshComponent>();
            for (auto entity : meshView) {
                auto& transformComponent = meshView.get<TransformComponent>(entity);
                auto& meshComponent = meshView.get<MeshComponent>(entity);

                meshComponent.mesh->UpdateAABB(transformComponent.accumulatedTransform);
            }
        }
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------

        
        //--MOUSE CHECKER
        void CheckRayModelIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::mat4& accumulatedTransform) 
        {
            auto view = m_registry->view<TransformComponent, MeshComponent>();
            for (auto entity : view) {
                auto& transformComponent = view.get<TransformComponent>(entity);
                auto& meshComponent = view.get<MeshComponent>(entity);

                glm::mat4 modelMatrix = accumulatedTransform * transformComponent.transform->getLocalModelMatrix();

                // Transformar AABB
                glm::vec3 transformedMin = glm::vec3(modelMatrix * glm::vec4(meshComponent.mesh->minBounds, 1.0));
                glm::vec3 transformedMax = glm::vec3(modelMatrix * glm::vec4(meshComponent.mesh->maxBounds, 1.0));

                // Verificar la intersección del rayo con la AABB transformada
                if (rayIntersectsBoundingBox(rayOrigin, rayDirection, transformedMin, transformedMax))
                {
                    entitiesInRay.push_back(entity);
                }
            }
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
        template<typename T>
        void RemoveComponent(entt::entity entity) {
            m_registry->remove<T>(entity);
        }
        //------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------




    private:
        // Constructor privado para el patrón Singleton
        EntityManager() {}


        // Función recursiva para crear entidades desde un modelo y sus hijos
        void CreateGameObjectFromModel(Ref<Model> model, entt::entity parent) 
        {
            // Crear una nueva entidad para el modelo
            entt::entity entity = CreateEmptyGameObject();
            auto& transformComponent = GetComponent<TransformComponent>(entity);
            transformComponent.transform = model->transform;


            // Asignar los componentes de MaterialComponent si el modelo tiene materiales
            if (!model->materials.empty()) {
                for (auto& material : model->materials) {
                    m_registry->emplace<MaterialComponent>(entity, material);
                }
            }

            // Asignar los componentes de MeshComponent si el modelo tiene mallas
            if (!model->meshes.empty()) {
                for (auto& mesh : model->meshes) {
                    m_registry->emplace<MeshComponent>(entity, mesh);
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


            // Asignar el ScriptComponent con ExampleScript
            //auto& scriptComponent = m_registry->emplace<ScriptComponent>(entity);
            //scriptComponent.instance = CreateRef<MyScript>();
            //scriptComponent.instance->SetEntity(entity, m_registry);
        }


        // Función de dibujo de GameObjects
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
            meshComponent.mesh->Draw();
        }





        bool rayIntersectsBoundingBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3 boxMin, glm::vec3 boxMax)
        {
            float tMin = (boxMin.x - rayOrigin.x) / rayDirection.x;
            float tMax = (boxMax.x - rayOrigin.x) / rayDirection.x;

            if (tMin > tMax) std::swap(tMin, tMax);

            float tyMin = (boxMin.y - rayOrigin.y) / rayDirection.y;
            float tyMax = (boxMax.y - rayOrigin.y) / rayDirection.y;

            if (tyMin > tyMax) std::swap(tyMin, tyMax);

            if ((tMin > tyMax) || (tyMin > tMax))
                return false;

            if (tyMin > tMin)
                tMin = tyMin;

            if (tyMax < tMax)
                tMax = tyMax;

            float tzMin = (boxMin.z - rayOrigin.z) / rayDirection.z;
            float tzMax = (boxMax.z - rayOrigin.z) / rayDirection.z;

            if (tzMin > tzMax) std::swap(tzMin, tzMax);

            if ((tMin > tzMax) || (tzMin > tMax))
                return false;

            if (tzMin > tMin)
                tMin = tzMin;

            if (tzMax < tMax)
                tMax = tzMax;

            return true;
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