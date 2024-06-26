#include "TransformComponent.h"

TransformComponent::TransformComponent() : ActorComponent()
{
	SetPosition(glm::vec3(0));
	SetScale(glm::vec3(1));
	SetRotation(glm::vec3(0));

}

glm::vec3 TransformComponent::direction()
{
	glm::vec3 forwardDir = glm::vec3(0, 0, -1);
	glm::vec3 direction = rotation * forwardDir;
	return glm::normalize(direction);
}

void TransformComponent::InitialConfiguration(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
	SetPosition(pos);
	SetScale(scale);
	SetRotation(rot);
}

void TransformComponent::SetScale(glm::vec3 newScale)
{
	scale = newScale;
}

void TransformComponent::SetPosition(glm::vec3 newPos)
{
	position = newPos;
}

void TransformComponent::SetRotation(glm::vec3 newRotation)
{
	rotation = newRotation;
}

void TransformComponent::Translation(glm::vec3 direction)
{
	position += direction;
}

void TransformComponent::Begin()
{
}

void TransformComponent::Tick(float DeltaTime)
{
}
