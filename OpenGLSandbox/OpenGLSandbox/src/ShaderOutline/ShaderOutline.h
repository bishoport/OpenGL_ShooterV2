#pragma once

#include <LibCore.h>
#include <tools/ShaderManager.h>
#include <tools/Camera.h>
#include <tools/PrimitivesHelper.h>

class ShaderOutline
{
public:
    ShaderOutline() = default;

	void Init();

private:

    libCore::ShaderManager shaderManager;

    float lastFrameTime = 0.0f;

    libCore::Camera* m_camera = nullptr;

    void LoopOpenGL(libCore::Timestep deltaTime);
    void OnCloseOpenGL();

    float outlineValue = 0.0300006699f;

    Ref<libCore::Mesh> testMesh;
};