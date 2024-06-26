#include "ActorComponent.h"
#include <iostream>


void ActorComponent::PreTick(float DeltaTime)
{
}

void ActorComponent::Ping(std::string message)
{
	std::cout << "Ping correctly to " << message << std::endl;
}
