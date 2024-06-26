#pragma once
#include "GameMode.h"

class IMGUIEditor {
public:
	Ref<Actor> selectedEntity;
 	void Init();

	void InspectorWindow();

	void SceneWindow();

	void LoadLayouts();
};