#include "MatrixStudioApp.h"
#include <input/InputManager.h>
#include <tools/TextureManager.h>

void MatrixStudio::Init()
{
    //--SETUP OPENGL & CALLBACKS
    bool ok = libCore::InitializeEngine("MATRIX STUDIO", 800, 600,
        std::bind(&MatrixStudio::LoopOpenGL, this, std::placeholders::_1),
        std::bind(&MatrixStudio::OnCloseOpenGL, this),
        std::bind(&MatrixStudio::LoopImGUI, this));
    if (!ok) return;
    //-----------------------------------------------------------------

    //--OpenGL FLAGS
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //---------------------------------------------------------------------------


    //-- SHADERS
    std::string shadersDirectory = "C:/Users/migue/Desktop/Projects/OpenGLSandbox/MatrixStudio/assets/shaders/";
    shaderManager.setShaderDataLoad("basic", shadersDirectory + "basic.vert", shadersDirectory + "basic.frag");
    shaderManager.setShaderDataLoad("text" , shadersDirectory + "text.vert" , shadersDirectory + "text.frag");
    shaderManager.LoadAllShaders();
    //-----------------------------------------------------------------


    //--FREETYPE
    freeTypeManager = new libCore::FreeTypeManager();
    //---------------------------------------------------------------------------

    //--WORLD AXIS
    axis3D = new CoordinateAXIS_3D();
    axis3D->SetAxisDirection(glm::vec3(1.0f, 1.0f, 1.0f));
    //---------------------------------------------------------------------------

    //-- PREPARE GEOMETRY
    
    //-----------------------------------------------------------------

    //-- PREPARE TEXTURES

    //-----------------------------------------------------------------

    //-- CAMERA
    m_camera = new libCore::Camera(800, 600, glm::vec3(0.0f, 0.0f, 5.0f));
    //-----------------------------------------------------------------



    //--TEST SUMA
    //glm::vec3 vectorA = glm::vec3(1.0f, 2.0f,0.0f);
    //glm::vec3 vectorB = glm::vec3(1.0f, -1.0f,2.0f);
    //glm::vec3 vectorR = vectorA + vectorB;

    //drawablesVec3.push_back(new DrawableVector3(vectorA, glm::vec3(1.0f, 0.0f, 0.0f)));
    //drawablesVec3.push_back(new DrawableVector3(vectorB, glm::vec3(0.0f, 0.0f, 1.0f)));
    //drawablesVec3.push_back(new DrawableVector3(vectorR, glm::vec3(0.0f, 1.0f, 0.0f)));
    //---------------------------------------------------------------------------
 


    //-- START LOOP OpenGL
    libCore::InitializeMainLoop();
    //------------------------------------------------------------------
}

std::string removeDecimals(float fToConvert, int decimals = 2)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(decimals) << fToConvert;
    return stream.str();
}

void MatrixStudio::LoopOpenGL(libCore::Timestep deltaTime)
{
    //INPUT
    if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_P))
    {
        if (m_wireframe == true)
        {
            m_wireframe = false;
            // Activar wireframe
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            m_wireframe = true;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }


    // Update CAMERA
    m_camera->Inputs(deltaTime);
    m_camera->updateMatrix(45.0f, 0.1f, 100.0f);

    // Clear
    glClearColor(0.3, 0.1, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(800), 0.0f, static_cast<float>(600));

    axis3D->DrawAxis3D();

    libCore::ShaderManager::Get("basic")->use();
    libCore::ShaderManager::Get("basic")->setMat4("view", m_camera->view);
    libCore::ShaderManager::Get("basic")->setMat4("projection", m_camera->projection);
    glm::mat4 model = glm::mat4(1.0f); // Transformación de modelo (identidad por defecto)
    libCore::ShaderManager::Get("basic")->setMat4("model", model);

    //// Dibujar todos los DrawableVector2
    for (size_t i = 0; i < drawablesVec3.size(); i++)
    {
        drawablesVec3[i]->SetAxisDirectionFactor(axis3D->axisDirection);
        drawablesVec3[i]->Draw(deltaTime);
    }


    libCore::ShaderManager::Get("text")->use();
    libCore::ShaderManager::Get("text")->setMat4("projection", m_camera->projection);
    libCore::ShaderManager::Get("text")->setMat4("model", model);
    libCore::ShaderManager::Get("text")->setMat4("view", m_camera->view);
    

    ////Current Axis Info (pasar a ImGUI)
    std::string currentAxisInfo = "{" + removeDecimals(axis3D->axisDirection.x) + "," + removeDecimals(axis3D->axisDirection.y) + "}";
    freeTypeManager->RenderText(currentAxisInfo, glm::vec3(-2.9f, 3.6f,0.0f), 0.003f, glm::vec3(1.0f, 1.0f, 1.0f));



    freeTypeManager->RenderText("+x", glm::vec3(3.5f, -0.2f,0.0f) * axis3D->axisDirection, 0.009, glm::vec3(1.0f, 1.0f, 1.0f));
    freeTypeManager->RenderText("+y", glm::vec3(0.2f, 3.5f,0.0f) * axis3D->axisDirection, 0.009, glm::vec3(1.0f, 1.0f, 1.0f));

    for (size_t i = 0; i < drawablesVec3.size(); i++)
    {
        std::string textInfo = "{" + removeDecimals(drawablesVec3[i]->vector.x) + "," + removeDecimals(drawablesVec3[i]->vector.y) + "," + removeDecimals(drawablesVec3[i]->vector.z) + "}";
        float yOffset = (drawablesVec3[i]->vector.y > 0) ? 0.15 : -0.25;
        freeTypeManager->RenderText(textInfo, glm::vec3(drawablesVec3[i]->vector.x, 
                                                        drawablesVec3[i]->vector.y + yOffset,
                                                        drawablesVec3[i]->vector.z) * axis3D->axisDirection, 0.003, glm::vec3(1.0f, 1.0f, 1.0f));
    }

}

void MatrixStudio::OnCloseOpenGL()
{
}

void MatrixStudio::LoopImGUI()
{
    // Tu código existente para la lista de vectores
    ImGui::Begin("Lista de Vectores");

    // Botón para crear un nuevo vector
    if (ImGui::Button("Crear Nuevo Vector")) {
        creatingNewVector = !creatingNewVector; // Alternar la visibilidad
    }

    // Sección para crear un nuevo vector
    if (creatingNewVector) {
        ImGui::Text("Valores del Nuevo Vector:");
        ImGui::SliderFloat("X", &newVectorValues.x, -10.0f, 10.0f); // Ajustar rangos según necesidad
        ImGui::SliderFloat("Y", &newVectorValues.y, -10.0f, 10.0f);

        ImGui::Text("Color del Vector:");
        ImGui::ColorEdit3("Color", &newVectorColor.x); // Selector de color

        // Botón para confirmar la creación del nuevo vector
        if (ImGui::Button("Confirmar Nuevo Vector")) {
            AddDrawableVector(newVectorValues, newVectorColor); // Llama a tu función para añadir el vector
            creatingNewVector = false; // Ocultar los controles después de añadir el vector
        }
    }
    for (int i = 0; i < drawablesVec3.size(); i++) {
        if (ImGui::Selectable(std::to_string(i).c_str(), selectedItem == i)) {
            selectedItem = i;
        }

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("VECTOR_PAYLOAD", &i, sizeof(int));
            ImGui::Text("Arrastrando Vec2 #%d", i);
            ImGui::EndDragDropSource();
        }
    }
    ImGui::End();




    // Tu código existente para propiedades del vector
    ImGui::Begin("Propiedades del Vector");
    if (selectedItem >= 0) {
        DrawableVector3& selectedVector = *drawablesVec3[selectedItem];
        ImGui::DragFloat2("##vector", &selectedVector.vector.x);
        ImGui::DragFloat2("##origin", &selectedVector.origin.x);
    }
    ImGui::End();




    // Nueva sección para operaciones de vectores
    ImGui::Begin("Operaciones de Vectores");


    ImGui::Text("Suma de Vectores");
    // Primer espacio para soltar el vector
    if (ImGui::Button(vectorSlot1 == -1 ? "Soltar Vector 1 Aquí" : ("Vector #" + std::to_string(vectorSlot1)).c_str())) {
        vectorSlot1 = -1;
    }
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("VECTOR_PAYLOAD")) {
            vectorSlot1 = *(const int*)payload->Data;
        }
        ImGui::EndDragDropTarget();
    }

    // Segundo espacio para soltar el vector
    if (ImGui::Button(vectorSlot2 == -1 ? "Soltar Vector 2 Aquí" : ("Vector #" + std::to_string(vectorSlot2)).c_str())) {
        vectorSlot2 = -1;
    }
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("VECTOR_PAYLOAD")) {
            vectorSlot2 = *(const int*)payload->Data;
        }
        ImGui::EndDragDropTarget();
    }


    // Botón para realizar la operación de suma
    if (ImGui::Button("Calcular Suma") && vectorSlot1 != -1 && vectorSlot2 != -1) {

        glm::vec3 result = drawablesVec3[vectorSlot1]->vector + drawablesVec3[vectorSlot2]->vector;
        AddDrawableVector(result, glm::vec3(0, 1, 0)); // Añadir el vector resultante

        // Movemos el segundo
        drawablesVec3[vectorSlot2]->setOriginTarget(drawablesVec3[vectorSlot1]->vector);
        drawablesVec3[vectorSlot2]->setVectorTarget(result);


        // Guardar los índices de los vectores usados y mostrar el botón de eliminación
        lastVector1Index = vectorSlot1;
        lastVector2Index = vectorSlot2;
        showRemoveButton = true;

        vectorSlot1 = -1; // Resetear los slots después de la operación
        vectorSlot2 = -1;
    }

    // Botón para eliminar los vectores usados en la suma
    if (showRemoveButton && ImGui::Button("Eliminar Vectores Usados")) {

        RemoveDrawableVector(lastVector1Index);
        RemoveDrawableVector(lastVector2Index);

        // Resetear las variables para no mostrar el botón nuevamente
        showRemoveButton = false;
        lastVector1Index = -1;
        lastVector2Index = -1;
    }
    ImGui::End();
}

void MatrixStudio::AddDrawableVector(const glm::vec3& vec, const glm::vec3& color)
{
    drawablesVec3.push_back(new DrawableVector3(vec, color));
}

void MatrixStudio::RemoveDrawableVector(int index)
{
    if (index >= 0 && index < drawablesVec3.size()) {
        drawablesVec3.erase(drawablesVec3.begin() + index);
    }
}
