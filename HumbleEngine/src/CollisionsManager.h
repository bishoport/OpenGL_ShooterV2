#pragma once
#include "Singleton.h"
#include "ActorSystem/Actor.h"

class CollisionsManager : public Singleton<CollisionsManager>{

	std::vector<Ref<ColliderComponent>> collidersList;

public:
	void Initialize();


	void Execute();

	void CollisionDetected();

	void AddCollider(Ref<Actor> actor);

	void AddMaxMinBounds(Actor* actor, glm::vec3 maxBounds, glm::vec3 minBounds);
};