#include "ColliderComponent.h"

void ColliderComponent::Begin()
{
}

void ColliderComponent::Tick(float deltaTime)
{
}

void ColliderComponent::ReceiveBounds(glm::vec3 min, glm::vec3 max)
{
	maxBound = max;
	minBound = min;
}

bool ColliderComponent::collisionAABB(Ref<ColliderComponent> otherComp)
{
	return (minBound.x <= otherComp->maxBound.x && maxBound.x >= otherComp->minBound.x) &&
		(minBound.y <= otherComp->maxBound.y && maxBound.y >= otherComp->minBound.y) &&
		(minBound.z <= otherComp->maxBound.z && maxBound.z >= otherComp->minBound.z);
}
