#pragma once

#include <LibCore.h>
#include <vector>

class CoordinateAXIS_3D
{
public:
    glm::vec3 axisDirection;

    CoordinateAXIS_3D();
    void DrawAxis3D();
    void SetAxisDirection(glm::vec3 direction);

private:
    GLuint axisVAO, axisVBO;
    float axisVertices[18] = {
        // Eje X, color rojo
        -10.0f, 0.0f, 0.0f, // Inicio del eje X
        10.0f, 0.0f, 0.0f, // Fin del eje X
        // Eje Y, color verde
        0.0f, -10.0f, 0.0f, // Inicio del eje Y
        0.0f,  10.0f, 0.0f, // Fin del eje Y
        // Eje Z, color azul
        0.0f, 0.0f, -10.0f, // Inicio del eje Z
        0.0f, 0.0f,  10.0f  // Fin del eje Z
    };

    GLuint axisMarksVAO, axisMarksVBO;
    std::vector<float> axisMarksVertices;

    void updateAxisVertices();
    void updateAxisMarksVertices();
};
