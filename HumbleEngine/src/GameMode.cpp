#include "GameMode.h"

void GameMode::BeginPlay()
{
	for (int i = 0; i < scenesActors.size(); i++) 
		scenesActors[i]->Begin();
	
}

void GameMode::PreRenderer()
{
}

void GameMode::Tick(float DeltaTime)
{
	for (int i = 0; i < scenesActors.size(); i++)
		scenesActors[i]->Tick(DeltaTime);
}

void GameMode::PostRenderer()
{
}
