#pragma once

#include <LibCore.h>
#include <tools/ShaderManager.h>
#include <tools/Camera.h>
#include <tools/PrimitivesHelper.h>

class Practica3
{
public:
    Practica3() = default;

    void Init();

private:

    libCore::ShaderManager shaderManager;

    float lastFrameTime = 0.0f;

    libCore::Camera* m_camera = nullptr;

    void LoopOpenGL(libCore::Timestep deltaTime);
    void OnCloseOpenGL();

    Ref<libCore::Texture> diffuse1;
    Ref<libCore::Texture> diffuse2;

    Ref<libCore::Mesh> testMesh1;
    Ref<libCore::Mesh> testMesh2;
};