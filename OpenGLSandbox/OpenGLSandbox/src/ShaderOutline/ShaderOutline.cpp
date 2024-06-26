#include "ShaderOutline.h"
#include <input/InputManager.h>


void ShaderOutline::Init()
{
    //--SETUP OPENGL & CALLBACKS
    bool ok = libCore::InitializeEngine("OUTLINE SHADER", 800, 600,
        std::bind(&ShaderOutline::LoopOpenGL, this, std::placeholders::_1),
        std::bind(&ShaderOutline::OnCloseOpenGL, this));

    if (!ok) return;

    glEnable(GL_STENCIL_TEST); // Habilita el test de stencil
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    //-- SHADERS
    std::string shadersDirectory = "C:/Users/pdortegon/Documents/PROPIOS/OpenGLSandbox_V1/OpenGLSandbox/OpenGLSandbox/src/ShaderOutline/shaders/";
    shaderManager.setShaderDataLoad("basic", 
                                    shadersDirectory + "basic.vert", 
                                    shadersDirectory + "basic.frag");

    shaderManager.setShaderDataLoad("outline", 
                                    shadersDirectory + "outline.vert",
                                    shadersDirectory + "outline.frag");

    shaderManager.LoadAllShaders();
    //-----------------------------------------------------------------


    //-- PREPARE GEOMETRY
    //testMesh = libCore::PrimitivesHelper::CreateCube();
    testMesh = libCore::PrimitivesHelper::CreateSphere(1.0,20,20);
    //-----------------------------------------------------------------

    //-- PREPARE TEXTURES
    std::string texturesDirectory = "C:/Users/pdortegon/Documents/PROPIOS/OpenGLSandbox_V1/OpenGLSandbox/OpenGLSandbox/assets/textures/";
    libCore::Texture diffuse = libCore::Texture((texturesDirectory + "floor_1.jpg").c_str(), "diffuse", 0);
    //-----------------------------------------------------------------

    //-- CAMERA
    m_camera = new libCore::Camera(800, 600, glm::vec3(0.0f, 0.0f, 5.0f));
    //-----------------------------------------------------------------

    //-- START LOOP OpenGL
    libCore::InitializeMainLoop();
    //------------------------------------------------------------------
}

void ShaderOutline::LoopOpenGL(libCore::Timestep deltaTime)
{
    //INPUT
    if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_T))
    {
        outlineValue += 0.01f;
    }
    else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_G))
    {
        outlineValue -= 0.01f;
    }

    // Update CAMERA
    m_camera->Inputs(deltaTime);
    m_camera->updateMatrix(45.0f, 0.1f, 100.0f);

    // Clear
    glClearColor(0.3, 0.1, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //Stencil 1
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // Configura el stencil para pasar siempre
    glStencilMask(0xFF);               // Habilita la escritura al buffer de stencil

    // --Draw first MODEL 
    libCore::ShaderManager::Get("basic")->use();
    libCore::ShaderManager::Get("basic")->setMat4("camMatrix", m_camera->cameraMatrix);
    libCore::ShaderManager::Get("basic")->setMat4("model", glm::mat4(1.0f));
    libCore::ShaderManager::Get("basic")->setMat4("translation", glm::mat4(1.0f));
    libCore::ShaderManager::Get("basic")->setMat4("rotation", glm::mat4(1.0f));
    libCore::ShaderManager::Get("basic")->setMat4("scale", glm::mat4(1.0f));
    testMesh->Draw();
    //---------------------------------------------------------------------------------------------------

    //Stencil 2
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // Dibuja sólo donde el stencil no es 1
    glStencilMask(0x00);                 // Desactiva la escritura al buffer de stencil
    glDisable(GL_DEPTH_TEST);            // Opcionalmente desactiva el test de profundidad para evitar z-fighting

    // --Draw second MODEL
    libCore::ShaderManager::Get("outline")->use();
    libCore::ShaderManager::Get("outline")->setMat4("camMatrix", m_camera->cameraMatrix);
    libCore::ShaderManager::Get("outline")->setMat4("model", glm::mat4(1.0f));
    libCore::ShaderManager::Get("outline")->setMat4("translation", glm::mat4(1.0f));
    libCore::ShaderManager::Get("outline")->setMat4("rotation", glm::mat4(1.0f));
    libCore::ShaderManager::Get("outline")->setMat4("scale", glm::mat4(1.0f));
    libCore::ShaderManager::Get("outline")->setFloat("outlining", outlineValue);
    testMesh->Draw();
    //---------------------------------------------------------------------------------------------------


    //Restaura los estados originales
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);
}


void ShaderOutline::OnCloseOpenGL(){}

