#pragma once

#include <LibCore.h>
#include <tools/ShaderManager.h>
#include <tools/Camera.h>
#include <tools/PrimitivesHelper.h>
#include <tools/ModelLoader.h>
#include <tools/FBOManager.h>
#include <tools/GBufferManager.h>
#include <Core/skeletal/animator.h>

class ExampleDeferred
{
public:
    ExampleDeferred() = default;
    void Init();

private:

    int screenWidth  = 800;
    int screenHeight = 600;

    Scope<libCore::FBOManager> fboManager;

    libCore::ShaderManager shaderManager;

    float lastFrameTime = 0.0f;

    libCore::Camera* m_camera = nullptr;

    void LoopOpenGL(libCore::Timestep deltaTime);
    void LoopImGUI();
    void OnCloseOpenGL();

    void LoadModelInScene(libCore::ImportModelData importModelData);

    std::vector<Ref<libCore::ModelContainer>> modelsInScene;


    float focusDepth = 1.0f;
    float blurRadius = 0.019f;
    float blurAmount  = 0.302f;
    int   blurSampleCount = 10;


    libCore::Animator* animator = nullptr;
};
