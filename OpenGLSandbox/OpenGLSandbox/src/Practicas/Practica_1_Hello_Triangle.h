#pragma once

#include <LibCore.h>

class Practica1
{
public:
	Practica1() = default;

	void Init();

private:

    float lastFrameTime = 0.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);

    void LoopOpenGL(libCore::Timestep deltaTime);
    void OnCloseOpenGL();


    void ShaderBuiderCompiler();
    void SetupVertexDataAttributes();

    const char* vertexShaderSource = R"glsl(
        #version 460 core
        layout (location = 0) in vec3 aPos;
        void main()
        {
           gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )glsl";

    const char* fragmentShaderSource = R"glsl(
        #version 460 core
        out vec4 FragColor;
        void main()
        {
           FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        }
    )glsl";

    unsigned int shaderProgram;


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[9] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    };

    unsigned int VBO, VAO;
};