#include "ImGUIEditor.h"
#include <LibCore.h>

void IMGUIEditor::Init()
{

}

void IMGUIEditor::InspectorWindow()
{
	ImGui::Begin("Inspector");

	if (selectedEntity != nullptr) {
		if (selectedEntity->ComponentExist<TransformComponent>()) {
			Ref<TransformComponent> transform = selectedEntity->GetComponent<TransformComponent>();
			ImGui::Text("Transform");

			ImGui::DragFloat3("Position", glm::value_ptr(transform->position), 0.01f);
			ImGui::DragFloat3("Scale", glm::value_ptr(transform->scale), 0.01f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(transform->rotation), 0.01f);
		}
		

	}

	ImGui::End();
}

void IMGUIEditor::SceneWindow()
{
	ImGui::Begin("Scene");
	if (GameMode::GetInstance().scenesActors.empty()) {
		ImGui::End();

		return;
	}

	for (int i = 0; i < GameMode::GetInstance().scenesActors.size(); i++) {
		Ref<Actor> actor = GameMode::GetInstance().scenesActors[i];
		if (actor->parent == nullptr) {
			std::string treeLabel = actor->ActorName;
			bool treeOpen = ImGui::MenuItem(treeLabel.c_str());
			if (ImGui::IsItemClicked())
			{
				if (selectedEntity != nullptr)
				{

				}
				selectedEntity = actor;
			}

		}
	}

	ImGui::End();

}

void IMGUIEditor::LoadLayouts()
{
	InspectorWindow();

	SceneWindow();
}
