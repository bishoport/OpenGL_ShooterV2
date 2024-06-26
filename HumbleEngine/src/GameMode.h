#pragma once
#include <vector>
#include "ActorSystem/Actor.h"
#include "Singleton.h"
class GameMode : public Singleton<GameMode>{

public:

	std::vector<Ref<Actor>> scenesActors;

	virtual void BeginPlay();

	virtual void PreRenderer();

	virtual void Tick(float DeltaTime);

	virtual void PostRenderer();


 };