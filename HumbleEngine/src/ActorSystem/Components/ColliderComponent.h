#pragma once
#include <LibCore.h>
#include "../ActorComponent.h"
#include <glm/glm.hpp>

class ColliderComponent : public ActorComponent {
	glm::vec3 maxBound;
	glm::vec3 minBound;

public:
	void Begin() override;

	void Tick(float deltaTime) override;

	void ReceiveBounds(glm::vec3 min, glm::vec3 max);

	bool collisionAABB(Ref<ColliderComponent> otherComp);
};