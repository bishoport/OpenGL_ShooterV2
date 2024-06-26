#include "Actor.h"



Actor::Actor(std::string Name)
{
	ActorName = Name;
	transform = std::make_shared<TransformComponent>();
	AddComponent(transform);
	std::random_device rd;
	std::mt19937 gen(rd());

	// Definir la distribución para números de 10 dígitos
	std::uniform_int_distribution<int> distribution(1000000000, 9999999999);

	// Generar y mostrar el número aleatorio
	int randomNumber = distribution(gen);
	ActorID = randomNumber;
}

ActorComponent& Actor::AddComponent(Ref<ActorComponent> Component)
{
	// TODO: Insertar una instrucción "return" aquí
	if (Component->IDActor == 0)
		Component->IDActor = ActorID;
	auto iterador = std::find(ComponentList.begin(), ComponentList.end(), Component);
	if (iterador == ComponentList.end()) {
		ComponentList.emplace_back(Component);
		return *Component;
	}
	Ref<ActorComponent> newComponent = *iterador;
	newComponent->owner = this;
	return *newComponent;
}



void Actor::Initialize(std::string Name, unsigned number)
{
	if (Name.length() <= 0) {
		int value = number + 1;
		ActorName = "ACTOR_" + std::to_string(value);
	}
	else {
		ActorName = Name;
	}
}

void Actor::PreRenderer()
{
}

void Actor::PostRenderer()
{
}

void Actor::Begin()
{
	for (int i = 0; i < ComponentList.size(); i++) 
		ComponentList[i]->Begin();
	
}



void Actor::Tick(float deltaTime)
{
	for (int i = 0; i < ComponentList.size(); i++)
		ComponentList[i]->Tick(deltaTime);
}

void Actor::OnColliderEnter(Ref<ColliderComponent> other)
{
	std::cout << "Cubic detection " << std::endl;
}

void Actor::ListComponents()
{
	for (int i = 0; i < ComponentList.size(); i++) {
		const std::type_info& type = typeid(*ComponentList[i]);
		std::cout << ComponentList[i] << " " << type.name() << std::endl;
	}

}








