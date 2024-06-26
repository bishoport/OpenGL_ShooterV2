#pragma once

#include <LibCore.h>

class Practica0
{
public:
	Practica0() = default;

	void Init();

private:

    float lastFrameTime = 0.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);

    void LoopOpenGL(libCore::Timestep deltaTime);
    void OnCloseOpenGL();
};