#include "CoordinateAXIS_3D.h"
#include <tools/ShaderManager.h>

CoordinateAXIS_3D::CoordinateAXIS_3D() : axisDirection(glm::vec3(1.0f, 1.0f, 1.0f))
{
    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);

    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Marcar cada 0.5 unidades en los ejes
    for (float i = -10.0f; i <= 10.0f; i += 0.5f) {
        // Líneas en el eje X
        axisMarksVertices.push_back(i);
        axisMarksVertices.push_back(-0.05f);
        axisMarksVertices.push_back(0.0f);
        axisMarksVertices.push_back(i);
        axisMarksVertices.push_back(0.05f);
        axisMarksVertices.push_back(0.0f);
        // Líneas en el eje Y
        axisMarksVertices.push_back(-0.05f);
        axisMarksVertices.push_back(i);
        axisMarksVertices.push_back(0.0f);
        axisMarksVertices.push_back(0.05f);
        axisMarksVertices.push_back(i);
        axisMarksVertices.push_back(0.0f);
        // Líneas en el eje Z
        axisMarksVertices.push_back(0.0f);
        axisMarksVertices.push_back(-0.05f);
        axisMarksVertices.push_back(i);
        axisMarksVertices.push_back(0.0f);
        axisMarksVertices.push_back(0.05f);
        axisMarksVertices.push_back(i);
    }

    glGenVertexArrays(1, &axisMarksVAO);
    glGenBuffers(1, &axisMarksVBO);

    glBindVertexArray(axisMarksVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisMarksVBO);
    glBufferData(GL_ARRAY_BUFFER, axisMarksVertices.size() * sizeof(float), axisMarksVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void CoordinateAXIS_3D::DrawAxis3D()
{
    libCore::ShaderManager::Get("basic")->use();
    libCore::ShaderManager::Get("basic")->setVec3("inputColor", glm::vec3(1.0f, 0.0f, 0.0f));
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 2);

    libCore::ShaderManager::Get("basic")->setVec3("inputColor", glm::vec3(0.0f, 1.0f, 0.0f));
    glDrawArrays(GL_LINES, 2, 2);

    libCore::ShaderManager::Get("basic")->setVec3("inputColor", glm::vec3(0.0f, 0.0f, 1.0f));
    glDrawArrays(GL_LINES, 4, 2);

    libCore::ShaderManager::Get("basic")->setVec3("inputColor", glm::vec3(1.0f, 1.0f, 1.0f));
    glBindVertexArray(axisMarksVAO);
    glDrawArrays(GL_LINES, 0, axisMarksVertices.size() / 3);

    glBindVertexArray(0);
}

void CoordinateAXIS_3D::SetAxisDirection(glm::vec3 direction) {
    axisDirection = direction;
    updateAxisVertices();
}

void CoordinateAXIS_3D::updateAxisVertices() {
    axisVertices[0] = -10.0f * axisDirection.x; // Inicio del eje X
    axisVertices[1] = 0.0f * axisDirection.y;
    axisVertices[2] = 0.0f * axisDirection.z;
    axisVertices[3] = 10.0f * axisDirection.x;  // Fin del eje X
    axisVertices[4] = 0.0f * axisDirection.y;
    axisVertices[5] = 0.0f * axisDirection.z;

    axisVertices[6]  =   0.0f * axisDirection.x;
    axisVertices[7]  = -10.0f * axisDirection.y; // Inicio del eje Y
    axisVertices[8]  =   0.0f * axisDirection.z;
    axisVertices[9]  =   0.0f * axisDirection.x;
    axisVertices[10] =  10.0f * axisDirection.y; // Fin del eje Y
    axisVertices[11] =   0.0f * axisDirection.z;

    axisVertices[12] =   0.0f * axisDirection.x;
    axisVertices[13] =   0.0f * axisDirection.y;
    axisVertices[14] = -10.0f * axisDirection.z; // Inicio del eje Z
    axisVertices[15] =   0.0f * axisDirection.x;
    axisVertices[16] =   0.0f * axisDirection.y;
    axisVertices[17] =  10.0f * axisDirection.z;  // Fin del eje Z

    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CoordinateAXIS_3D::updateAxisMarksVertices() {
    axisMarksVertices.clear();

    for (float i = -10.0f; i <= 10.0f; i += 0.5f) {
        // Líneas en el eje X
        axisMarksVertices.push_back(i);
        axisMarksVertices.push_back(-0.05f);
        axisMarksVertices.push_back(0.0f);
        axisMarksVertices.push_back(i);
        axisMarksVertices.push_back(0.05f);
        axisMarksVertices.push_back(0.0f);
        // Líneas en el eje Y
        axisMarksVertices.push_back(-0.05f);
        axisMarksVertices.push_back(i);
        axisMarksVertices.push_back(0.0f);
        axisMarksVertices.push_back(0.05f);
        axisMarksVertices.push_back(i);
        axisMarksVertices.push_back(0.0f);
        // Líneas en el eje Z
        axisMarksVertices.push_back(0.0f);
        axisMarksVertices.push_back(-0.05f);
        axisMarksVertices.push_back(i);
        axisMarksVertices.push_back(0.0f);
        axisMarksVertices.push_back(0.05f);
        axisMarksVertices.push_back(i);
    }

    glBindBuffer(GL_ARRAY_BUFFER, axisMarksVBO);
    glBufferData(GL_ARRAY_BUFFER, axisMarksVertices.size() * sizeof(float), axisMarksVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

