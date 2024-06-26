#pragma once
#include "../ActorComponent.h"
#include <LibCore.h>
#include "TransformComponent.h"
#include <tools/Camera.h>
#include <Core/Model.h>

class RendererComponent : public ActorComponent {
public:
	Ref<Mesh> CurrentMesh;
	Ref<Texture> mainTexture;
	
	Ref<libCore::Shader> CurrentShader;
	Ref<libCore::Camera> CurrentCamera;
	Ref<TransformComponent> Transform;

	RendererComponent(Ref<TransformComponent> newTransform);

	glm::mat4 projection();

	glm::mat4 view();

	glm::mat4 model();

	void Begin() override;

	void Tick(float deltaTime) override;

	void RendererProcess(bool isLightingOnlyBool);

	void SetModelInfo(Ref<Mesh> model, Ref<libCore::Shader> shader, Ref<libCore::Camera> camera, Ref<Texture> texture);

	void RenderBoundingBox(glm::mat4 model);


private:

};