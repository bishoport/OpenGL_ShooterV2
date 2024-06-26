#pragma once
#include "../ActorComponent.h"
#include <glm/glm.hpp>

class TransformComponent : public ActorComponent {
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	TransformComponent();

	glm::vec3 direction();

	void InitialConfiguration(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);

	void SetScale(glm::vec3 newScale);
	void SetPosition(glm::vec3 newPos);

	void SetRotation(glm::vec3 newRotation);

	void Translation(glm::vec3 direction);

	void Begin() override;

	void Tick(float DeltaTime) override;
};