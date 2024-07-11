#include "MapEditor.h"
#include <Core/EngineOpenGL.h>
#include <tools/LightsManager.hpp>


void MapEditor::Init()
{
    // -- PREPARE ENGINE
    bool ok = libCore::EngineOpenGL::GetInstance().InitializeEngine("MAP EDITOR", screenWidth, screenHeight,
        std::bind(&MapEditor::LoopOpenGL, this, std::placeholders::_1),
        std::bind(&MapEditor::OnCloseOpenGL, this),
        std::bind(&MapEditor::LoopImGUI, this));
    if (!ok) return;
    //-----------------------------------------------------------------


    // -- PREPARE GEOMETRY
    //libCore::ImportModelData importModelData;
    //importModelData.filePath = "assets/models/GLTF/scifi/";
    //importModelData.fileName = "scene.gltf";
    //importModelData.invertUV = false;
    //importModelData.rotate90 = true;
    //importModelData.useCustomTransform = true;
    //importModelData.modelID = 1;
    //importModelData.globalScaleFactor = 1.0f;
    //libCore::EngineOpenGL::GetInstance().CreatePrefabExternalModel(importModelData);


    libCore::ImportModelData importModelData;
    importModelData.filePath = "assets/models/Robot/";
    importModelData.fileName = "Robot.fbx";
    importModelData.invertUV = false;
    importModelData.rotate90 = false;
    importModelData.useCustomTransform = true;
    importModelData.modelID = 1;
    importModelData.globalScaleFactor = 1.0f;
    libCore::EngineOpenGL::GetInstance().CreatePrefabExternalModel(importModelData);

    //libCore::ImportModelData importModelData2;
    //importModelData2.filePath = "assets/models/Turbina/";
    //importModelData2.fileName = "turbina.fbx";
    //importModelData2.invertUV = false;
    //importModelData2.rotate90 = false;
    //importModelData2.useCustomTransform = true;
    //importModelData2.modelID = 1;
    //importModelData2.globalScaleFactor = 0.3f;
    //libCore::EngineOpenGL::GetInstance().CreatePrefabExternalModel(importModelData2);

    //libCore::ImportModelData importModelData3;
    //importModelData3.filePath = "assets/models/Corridor/Pasillo1/";
    //importModelData3.fileName = "Pasillo1.fbx";
    //importModelData3.invertUV = false;
    //importModelData3.rotate90 = false;
    //importModelData3.useCustomTransform = true;
    //importModelData3.modelID = 1;
    //importModelData3.globalScaleFactor = 1.0f;
    //importModelData3.processLights = false;
    //libCore::EngineOpenGL::GetInstance().CreatePrefabExternalModel(importModelData3);

    //libCore::ImportModelData importModelData3;
    //importModelData3.filePath = "assets/models/Castle/";
    //importModelData3.fileName = "sponza.obj";
    //importModelData3.invertUV = false;
    //importModelData3.rotate90 = false;
    //importModelData3.useCustomTransform = true;
    //importModelData3.modelID = 1;
    //importModelData3.globalScaleFactor = 0.4f;
    //importModelData3.processLights = false;
    //libCore::EngineOpenGL::GetInstance().CreatePrefabExternalModel(importModelData3);

    //libCore::ImportModelData importModelData4;
    //importModelData4.filePath = "assets/models/LightTestScene/";
    //importModelData4.fileName = "Scene.fbx";
    //importModelData4.invertUV = false;
    //importModelData4.rotate90 = false;
    //importModelData4.useCustomTransform = true;
    //importModelData4.modelID = 1;
    //importModelData4.globalScaleFactor = 1.0f;
    //importModelData4.processLights = false;
    //libCore::EngineOpenGL::GetInstance().CreatePrefabExternalModel(importModelData4);

    libCore::ImportModelData importModelData4;
    importModelData4.filePath = "assets/models/Scenario_1/";
    importModelData4.fileName = "CorridorModule_1.fbx";
    importModelData4.invertUV = false;
    importModelData4.rotate90 = false;
    importModelData4.useCustomTransform = true;
    importModelData4.modelID = 1;
    importModelData4.globalScaleFactor = 1.0f;
    importModelData4.processLights = true;
    libCore::EngineOpenGL::GetInstance().CreatePrefabExternalModel(importModelData4);

    //libCore::ImportModelData importModelData4;
    //importModelData4.filePath = "assets/models/";
    //importModelData4.fileName = "Pos_test.fbx";
    //importModelData4.invertUV = false;
    //importModelData4.rotate90 = false;
    //importModelData4.useCustomTransform = true;
    //importModelData4.modelID = 1;
    //importModelData4.globalScaleFactor = 1.0f;
    //importModelData4.processLights = true;
    //libCore::EngineOpenGL::GetInstance().CreatePrefabExternalModel(importModelData4);

    //libCore::ImportModelData importModelData4;
    //importModelData4.filePath = "assets/models/StarWars/";
    //importModelData4.fileName = "StarWarsCargo_Scene_AssetKit.fbx";
    //importModelData4.invertUV = false;
    //importModelData4.rotate90 = false;
    //importModelData4.useCustomTransform = true;
    //importModelData4.modelID = 1;
    //importModelData4.globalScaleFactor = 0.3f;
    //importModelData4.processLights = true;
    //libCore::EngineOpenGL::GetInstance().CreatePrefabExternalModel(importModelData4);


    //libCore::ImportModelData importModelData4;
    //importModelData4.filePath = "assets/models/CyberpunkStreet/";
    //importModelData4.fileName = "CyberpunkStreet.fbx";
    //importModelData4.invertUV = false;
    //importModelData4.rotate90 = false;
    //importModelData4.useCustomTransform = true;
    //importModelData4.modelID = 1;
    //importModelData4.globalScaleFactor = 0.3f;
    //importModelData4.processLights = true;
    //libCore::EngineOpenGL::GetInstance().CreatePrefabExternalModel(importModelData4);

    //libCore::EngineOpenGL::GetInstance().CreatePrefabCube(glm::vec3(0.0f, 0.0f, 0.0f));

    //libCore::EngineOpenGL::GetInstance().CreateTriangle(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f));
    //libCore::EngineOpenGL::GetInstance().CreateTriangle(glm::vec3(0.0f, 1.0f, 0.0f),glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, 0.0f));
    //------------------------------------------------------------------
    

    // -- EXTRA LIGHTS
    //libCore::LightsManager::CreateDirectionalLight();
    //libCore::LightsManager::CreateLight(true, libCore::LightType::POINT, glm::vec3(0.0f, 15.0f, 0.0f));
    //libCore::LightsManager::CreateLight(true, libCore::LightType::AREA, glm::vec3(5.0f, 15.0f, 0.0f));
    //------------------------------------------------------------------


    // -- VIEWPORTS
    libCore::EngineOpenGL::GetInstance().CreateViewport("EDITOR CAMERA", glm::vec3(0.0f, 20.0f, 0.0f), CAMERA_CONTROLLERS::EDITOR);
    libCore::EngineOpenGL::GetInstance().CreateViewport("GAME CAMERA", glm::vec3(0.0f, 15.0f, 0.0f), CAMERA_CONTROLLERS::FPS);
    //------------------------------------------------------------------


    // -- START LOOP OpenGL
    libCore::EngineOpenGL::GetInstance().InitializeMainLoop();
    //------------------------------------------------------------------
}




void MapEditor::LoopOpenGL(libCore::Timestep deltaTime){}
void MapEditor::LoopImGUI(){}
void MapEditor::OnCloseOpenGL(){}
