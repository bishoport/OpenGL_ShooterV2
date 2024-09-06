#include "MapEditor.h"
#include <Core/EngineOpenGL.h>
#include <tools/LightsManager.hpp>


void MapEditor::Init()
{
    // -- PREPARE ENGINE
    bool ok = libCore::EngineOpenGL::GetInstance().InitializeEngine("MAP EDITOR", screenWidth, screenHeight);
    if (!ok) return;
    //-----------------------------------------------------------------

    // -- VIEWPORTS
    libCore::EngineOpenGL::GetInstance().CreateViewport("EDITOR CAMERA", glm::vec3(0.0f, 20.0f, 0.0f), CAMERA_CONTROLLERS::EDITOR);
    //------------------------------------------------------------------

    // -- START LOOP OpenGL
    libCore::EngineOpenGL::GetInstance().InitializeMainLoop();
    //------------------------------------------------------------------
}
