#include "Practica_0_Template.h"
#include <input/InputManager.h>

void Practica0::Init()
{
    //--SETUP OPENGL & CALLBACKS
    bool ok = libCore::InitializeEngine("Practica 0 Hello GLWorld", 800, 600,
        std::bind(&Practica0::LoopOpenGL, this, std::placeholders::_1),
        std::bind(&Practica0::OnCloseOpenGL, this));

    if (!ok) return;


    //--START LOOP OpenGL
    libCore::InitializeMainLoop();
    //----------------------------------------------------------------------------
}

void Practica0::LoopOpenGL(libCore::Timestep deltaTime)
{
    //INPUT
    if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_D))
    {
        std::cout << "HELLO GL WORLD" << std::endl;
    }

    // Clear
    glClearColor(0.3, 0.1, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Practica0::OnCloseOpenGL()
{

}