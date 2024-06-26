#pragma once

#include <LibCore.h>
#include <tools/FreeTypeManager.h>
#include <tools/Camera.h>
#include "ActorSystem/Actor.h"
#include "Scenes/Scene.h"
#include "ImGUIEditor.h"

class HumbleEngineApp {

public:
	HumbleEngineApp() = default;

	void Init();

private:
    libCore::ShaderManager shaderManager;
    Ref<Scene> currentScene;
    Ref<IMGUIEditor> imguiEditor;
    libCore::FreeTypeManager* freeTypeManager = nullptr;
    libCore::Camera* m_camera = nullptr;

    float lastFrameTime = 0.0f;

    void LoopOpenGL(libCore::Timestep deltaTime);
    void OnCloseOpenGL();
    void LoopImGUI();

    Ref<Texture> diffuse1;
    Ref<libCore::Camera> newCamera;

};