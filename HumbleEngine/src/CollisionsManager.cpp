#include "CollisionsManager.h"

void CollisionsManager::Initialize()
{
}

void CollisionsManager::Execute()
{
	CollisionDetected();
}

void CollisionsManager::CollisionDetected()
{
	for (int i = 0; i < collidersList.size(); i++) {
		for (int j = 0; j < collidersList.size(); j++) {
			if (collidersList[i] == collidersList[j])
				continue;
			if (collidersList[i]->collisionAABB(collidersList[j])) {
				collidersList[i]->owner->OnColliderEnter(collidersList[j]);
			}
		}
	}
}

void CollisionsManager::AddCollider(Ref<Actor> actor)
{
	if (actor->ComponentExist<ColliderComponent>()) {
		Ref<ColliderComponent> collider = actor->GetComponent<ColliderComponent>();
		collider->owner = actor.get();
		collidersList.emplace_back(collider);
	}
}

void CollisionsManager::AddMaxMinBounds(Actor* actor, glm::vec3 maxBounds, glm::vec3 minBounds)
{
	actor->GetComponent<ColliderComponent>()->ReceiveBounds(maxBounds, minBounds);
}
