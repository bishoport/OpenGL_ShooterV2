#pragma once

#include <LibCore.h>
#include <tools/ShaderManager.h>
#include <tools/Camera.h>
#include "CoordinateAXIS_3D.h"
#include <tools/FreeTypeManager.h>
#include "Drawables.h"

class MatrixStudio
{
public:
    MatrixStudio() = default;

    void Init();

private:
    bool m_wireframe = false;
    CoordinateAXIS_3D* axis3D = nullptr;

    libCore::ShaderManager shaderManager;
    libCore::FreeTypeManager* freeTypeManager = nullptr;
    std::vector<DrawableVector3*> drawablesVec3;
    float lastFrameTime = 0.0f;

    libCore::Camera* m_camera = nullptr;
    void LoopOpenGL(libCore::Timestep deltaTime);
    void OnCloseOpenGL();
    void LoopImGUI();



    //PARA LA CALCULADORA:
    void AddDrawableVector(const glm::vec3& vec, const glm::vec3& color);
    void RemoveDrawableVector(int index);

    int selectedItem = -1; // Índice del elemento seleccionado en la lista
    int vectorSlot1 = -1;  // Índice para el primer vector en la operación de suma
    int vectorSlot2 = -1;  // Índice para el segundo vector en la operación de suma

    bool creatingNewVector = false; // Para controlar la visibilidad de la sección de creación
    glm::vec3 newVectorValues = glm::vec3(0.0f, 0.0f,0.0f); // Valores iniciales del nuevo vector
    glm::vec3 newVectorColor = glm::vec3(1.0f, 0.0f, 0.0f); // Color inicial del nuevo vector (rojo)

    bool showRemoveButton = false;
    int lastVector1Index = -1;
    int lastVector2Index = -1;
};