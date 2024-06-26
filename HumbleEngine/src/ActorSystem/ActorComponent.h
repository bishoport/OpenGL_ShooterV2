#pragma once
#include <string>
class Actor;
class ActorComponent {
public:
	int IDActor;
	Actor* owner;
	virtual void Begin() = 0;


	virtual void PreTick(float DeltaTime);
	virtual void Tick(float DeltaTime) = 0;
	bool rendererBool;

	void Ping(std::string message);
};