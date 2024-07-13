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

namespace libCore {

    //// Define el tipo de función para la creación de scripts
    //typedef libCore::Script* (*CreateScriptFunc)();

    class EntityManager {
    public:

        // Método para obtener la instancia de la clase
        static EntityManager& GetInstance() {
            static EntityManager instance;
            return instance;
        }

        void LoadAndUseDLL() {
            //// Cargar la DLL
            //HMODULE hModule = LoadLibrary(L"ScriptLibrary.dll");
            //if (!hModule) {
            //    std::cerr << "No se pudo cargar la DLL" << std::endl;
            //    return;
            //}

            //// Obtener la dirección de la función de creación de instancia
            //CreateScriptFunc createExampleScript = (CreateScriptFunc)GetProcAddress(hModule, "CreateExampleScript");
            //if (!createExampleScript) {
            //    std::cerr << "No se pudo obtener la dirección de CreateExampleScript" << std::endl;
            //    FreeLibrary(hModule);
            //    return;
            //}

            //// Usar la función para crear una instancia de ExampleScript
            //libCore::Script* scriptInstance = createExampleScript();
            //if (!scriptInstance) {
            //    std::cerr << "No se pudo crear la instancia del script" << std::endl;
            //    FreeLibrary(hModule);
            //    return;
            //}

            //// Configurar la entidad y el registro
            //auto registry = libCore::EntityManager::GetInstance().m_registry;
            //entt::entity entity = registry->create();

            //scriptInstance->SetEntity(entity, registry);

            //// Asignar ScriptComponent con la instancia creada
            //auto& scriptComponent = registry->emplace<libCore::ScriptComponent>(entity);
            //scriptComponent.instance.reset(scriptInstance);

            //// Inicializar y actualizar el script
            //scriptComponent.Init();
            //scriptComponent.Update(0.016f); // Ejemplo de actualización con deltaTime

            //// Liberar la DLL cuando ya no se necesite
            //FreeLibrary(hModule);
        }



        //--ENTITY COLLECTIONS
        Ref<entt::registry> m_registry = CreateRef<entt::registry>();
        std::vector<entt::entity> entitiesInRay;
        entt::entity currentSelectedEntityInScene;


        //--CREADOR DE ENTITIES
        void CreatePrefabExternalModel(ImportModelData importModelData) {
            Ref<Model> model = ModelLoader::LoadModel(importModelData);
            CreateEntityFromModel(model, entt::null);
            model.reset();
        }

        //--PINTADOR DE ENTITIES
        void DrawEntities(const std::string& shader) {
            auto view = m_registry->view<TransformComponent, MeshComponent, MaterialComponent>();
            for (auto entity : view) {
                auto& transform = view.get<TransformComponent>(entity);
                auto& mesh = view.get<MeshComponent>(entity);
                auto& material = view.get<MaterialComponent>(entity);

                // Aquí llamamos a la función de dibujo de tu motor gráfico
                DrawEntity(transform, mesh, material, shader);
            }
        }
        void DrawABBEntities(const std::string& shader) {
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


        //--ACTUALIZADOR DE FUNCIONES UPDATES ANTES DEL RENDER DE LOS COMPONENTES
        void UpdateEntities(Timestep deltaTime) {

            //// Inicializar y actualizar scripts
            //auto scriptView = m_registry->view<ScriptComponent>();
            //for (auto entity : scriptView) {
            //    auto& scriptComponent = scriptView.get<ScriptComponent>(entity);
            //    scriptComponent.Update(deltaTime.GetMilliseconds());
            //}

            auto view = m_registry->view<TransformComponent, MeshComponent>();
            for (auto entity : view) {
                auto& transformComponent = view.get<TransformComponent>(entity);
                auto& meshComponent = view.get<MeshComponent>(entity);

                transformComponent.transform->updateRotationFromEulerAngles();
                meshComponent.mesh->UpdateAABB(transformComponent.transform->getLocalModelMatrix());
            }
        }

        
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


        // Función genérica para obtener un componente de una entidad
        template<typename T>
        T& GetComponent(entt::entity entity) {
            return m_registry->get<T>(entity);
        }


    private:
        // Constructor privado para el patrón Singleton
        EntityManager() {}


        // Función recursiva para crear entidades desde un modelo y sus hijos
        void CreateEntityFromModel(Ref<Model> model, entt::entity parent) {
            // Crear una nueva entidad para el modelo
            entt::entity entity = m_registry->create();

            // Asignar el componente TransformComponent
            auto& transformComponent = m_registry->emplace<TransformComponent>(entity);
            transformComponent.transform = model->transform;

            // Asignar el ScriptComponent con ExampleScript
            //auto& scriptComponent = m_registry->emplace<ScriptComponent>(entity);
            //scriptComponent.instance = CreateRef<ExampleScript>();
            //scriptComponent.instance->SetEntity(entity, m_registry);

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
                CreateEntityFromModel(child, entity);
            }
        }

        // Función de dibujo de entidades
        void DrawEntity(TransformComponent& transformComponent, MeshComponent& meshComponent, MaterialComponent& materialComponent, const std::string& shader)
        {
            //VALUES
            libCore::ShaderManager::Get(shader)->setVec3("albedoColor", materialComponent.material->albedoColor);
            libCore::ShaderManager::Get(shader)->setFloat("normalStrength", materialComponent.material->normalStrength);
            libCore::ShaderManager::Get(shader)->setFloat("metallicValue", materialComponent.material->metallicValue);
            libCore::ShaderManager::Get(shader)->setFloat("roughnessValue", materialComponent.material->roughnessValue);

            //TEXTURES
            materialComponent.material->albedoMap->Bind(shader);
            materialComponent.material->normalMap->Bind(shader);
            materialComponent.material->metallicMap->Bind(shader);
            materialComponent.material->roughnessMap->Bind(shader);
            
            libCore::ShaderManager::Get(shader)->setMat4("model", transformComponent.transform->getMatrix());
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