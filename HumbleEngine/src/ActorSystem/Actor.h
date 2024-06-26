#pragma once 
#include <string>
#include <random>
#include "ActorComponent.h"
#include "Components/TransformComponent.h"
#include "Components/ColliderComponent.h"

class Actor {
public:
	std::string ActorName;
	Ref<Actor> parent;
	unsigned ActorID;
	

public:
	Actor(std::string Name = "");
	ActorComponent& AddComponent(Ref<ActorComponent> Component);
	//template<typename T> T& GetComponent();

	
	template<typename T>
	inline Ref<T> GetComponent()
	{
		for (int i = 0; i < ComponentList.size(); i++) {
			Ref<ActorComponent> actorComp = ComponentList[i];
			if (std::dynamic_pointer_cast<T>(actorComp) != nullptr) {
				return std::dynamic_pointer_cast<T>(ComponentList[i]);
			}
		}
		throw std::runtime_error("Component of type T not found");
		// TODO: Insertar una instrucción "return" aquí
	}

	
	template<typename T>
	bool ComponentExist()
	{
		for (int i = 0; i < ComponentList.size(); i++) {
			Ref<ActorComponent> actorComp = ComponentList[i];
			if (std::dynamic_pointer_cast<T>(actorComp) != nullptr)
				return true;

		}
		return false;
	}

	template<typename T> void TryGetComponent()
	{
		std::cout << "Try Get Component " << std::endl;
	}
	std::vector<Ref<ActorComponent>> ComponentList;

	Ref<TransformComponent> transform;

	virtual void Initialize(std::string Name = nullptr, unsigned number = 0);

	virtual void PreRenderer();

	virtual void PostRenderer();

	virtual void Begin();

	virtual void Tick(float deltaTime);

	virtual void OnColliderEnter(Ref<ColliderComponent> other);

	void ListComponents();

};
