#include "ExampleDeferred.h"
#include <input/InputManager.h>
#include <tools/TextureManager.h>
#include <tools/ModelLoader.h>

#include <Core/skeletal/animation.h>
#include <tools/SkeletalModelLoader.h>


void ExampleDeferred::Init()
{
    // -- SETUP OPENGL & CALLBACKS
    bool ok = libCore::InitializeEngine("Deferred Example", screenWidth, screenHeight,
        std::bind(&ExampleDeferred::LoopOpenGL, this, std::placeholders::_1),
        std::bind(&ExampleDeferred::OnCloseOpenGL, this),
        std::bind(&ExampleDeferred::LoopImGUI, this));
    if (!ok) return;
    //-----------------------------------------------------------------

    // -- OPENGL FLAGS
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //-----------------------------

    // -- SHADERS
    std::string shadersDirectory = "assets/shaders/";
    shaderManager.setShaderDataLoad("basic",         shadersDirectory + "basic.vert",         shadersDirectory + "basic.frag");
    shaderManager.setShaderDataLoad("depthQuadFBO",  shadersDirectory + "quad_fbo.vert",      shadersDirectory + "depth_quad_fbo.frag");
    shaderManager.setShaderDataLoad("colorQuadFBO",  shadersDirectory + "quad_fbo.vert",      shadersDirectory + "color_quad_fbo.frag");
    
    shaderManager.LoadAllShaders();
    //-----------------------------------------------------------------


    // -- PREPARE GEOMETRY
    //libCore::ImportModelData importModelData2;
    //importModelData2.filePath = "assets/models/vampire/";
    //importModelData2.fileName = "dancing_vampire.dae";
    //importModelData2.invertUV = false;
    //importModelData2.rotate90 = false;
    //importModelData2.skeletal = true;
    //importModelData2.useCustomTransform = false;
    //importModelData2.modelID = 1;
    //importModelData2.globalScaleFactor = 1.0f;
    //LoadModelInScene(importModelData2);


    libCore::ImportModelData importModelData1;
    importModelData1.filePath = "assets/models/varios/";
    importModelData1.fileName = "aj.dae";
    importModelData1.invertUV = false;
    importModelData1.rotate90 = false;
    importModelData1.skeletal = true;
    importModelData1.useCustomTransform = false;
    importModelData1.modelID = 1;
    importModelData1.globalScaleFactor = 1.0f;
    LoadModelInScene(importModelData1);



    //libCore::ImportModelData importModelData;
    //importModelData.filePath = "assets/models/Robot/";
    //importModelData.fileName = "Robot.fbx";
    //importModelData.invertUV = false;
    //importModelData.rotate90 = false;
    //importModelData.skeletal = false;
    //importModelData.modelID = 0;
    //importModelData.globalScaleFactor = 1.0f;
    //LoadModelInScene(importModelData);

    //libCore::ImportModelData importModelData;
    //importModelData.filePath = "assets/models/Castle/";
    //importModelData.fileName = "sponza.obj";
    //importModelData.invertUV = false;
    //importModelData.rotate90 = false;
    //importModelData.useCustomTransform = true;
    //importModelData.modelID = 1;
    //importModelData.globalScaleFactor = 0.1f;
    //LoadModelInScene(importModelData);

    
    //-----------------------------------------------------------------


    // -- CAMERA
    m_camera = new libCore::Camera(screenWidth, screenHeight, glm::vec3(0.0f, 0.0f, 5.0f));
    //-----------------------------------------------------------------


    // -- FRAME BUFFER MANAGER
    fboManager = CreateScope<libCore::FBOManager>();
    fboManager->init(screenWidth, screenHeight);
    //------------------------------------------------------------------


    libCore::Animation danceAnimation("assets/models/varios/jump.dae", modelsInScene[0]->UnifyMeshes());
    animator = new libCore::Animator(&danceAnimation);


    // -- START LOOP OpenGL
    libCore::InitializeMainLoop();
    //------------------------------------------------------------------
}



void ExampleDeferred::LoopOpenGL(libCore::Timestep deltaTime)
{
    // Update CAMERA
    m_camera->Inputs(deltaTime);
    m_camera->updateMatrix(45.0f, 0.1f, 1000.0f);
    
    
    animator->UpdateAnimation(deltaTime);


  
    glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
    fboManager->bindFBO();

    // Clear
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    //-- Normal Draw
    libCore::ShaderManager::Get("basic")->use();
    libCore::ShaderManager::Get("basic")->setMat4("camMatrix", m_camera->cameraMatrix);

    for (auto& modelContainer : modelsInScene) {

        libCore::ShaderManager::Get("basic")->setBool("use_skeletal", modelContainer->skeletal);

        if (modelContainer->skeletal == true)
        {
            auto transforms = animator->GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i)
                libCore::ShaderManager::Get("basic")->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }
        modelContainer->Draw("basic");
    }
    //---------------------------


    fboManager->unbindFBO(); 



    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


   // Dibujar color attachment
   libCore::ShaderManager::Get("colorQuadFBO")->use();
   libCore::ShaderManager::Get("colorQuadFBO")->setInt("screenTexture", 0);
   libCore::ShaderManager::Get("colorQuadFBO")->setInt("depthTexture" , 1);
   libCore::ShaderManager::Get("colorQuadFBO")->setFloat("focusDepth" , focusDepth);
   libCore::ShaderManager::Get("colorQuadFBO")->setFloat("blurRadius" , blurRadius);
   libCore::ShaderManager::Get("colorQuadFBO")->setFloat("blurAmount" , blurAmount);
   libCore::ShaderManager::Get("colorQuadFBO")->setInt("sampleCount"  , blurSampleCount);

   fboManager->bindTexture("color",0);
   fboManager->bindTexture("depth",1);

   fboManager->drawFBO("color");


    // Si necesitas visualizar el depth attachment:
    //libCore::ShaderManager::Get("depthQuadFBO")->use();
    //libCore::ShaderManager::Get("depthQuadFBO")->setFloat("near", 0.1f);
    //libCore::ShaderManager::Get("depthQuadFBO")->setFloat("far", 5.0f);
    //libCore::ShaderManager::Get("depthQuadFBO")->setInt("depthTexture", 0);
    //fboManager->drawFBO("depth");
}

void ExampleDeferred::LoopImGUI()
{
    // Crear una ventana ImGui y un panel
    ImGui::Begin("Float Values Control Panel"); // Comienza una ventana nueva

    ImGui::SliderFloat("focusDepth", &focusDepth, 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("blurRadius", &blurRadius, 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("blurAmount", &blurAmount, 0.0f, 1.0f, "%.3f");
    ImGui::SliderInt("Sample Count", &blurSampleCount, 1, 20); // Puedes ajustar los valores mínimos y máximos según lo necesario

    ImGui::End(); // Finaliza la ventana


    ImGui::Begin("Hierarchy");

    for (auto& modelContainer : modelsInScene) {

        if (ImGui::TreeNode(modelContainer.get(), "Model: %s", modelContainer->name.c_str())) {
            for (int j = 0; j < modelContainer->models.size(); j++) {
                if (ImGui::TreeNode(modelContainer->models[j].get(), "Child: %d", j)) {
                    // Aquí listamos las Meshes de cada Model
                    auto& model = modelContainer->models[j];
                    for (int k = 0; k < model->meshes.size(); k++) {
                        ImGui::BulletText("Mesh: %s", model->meshes[k]->meshName.c_str());
                        ImGui::BulletText("Transform:");
                        ImGui::DragFloat3("Position", &model->transform.position[0], 0.1f);
                        ImGui::DragFloat3("Rotation", &model->transform.rotation[0], 0.01f, -3.14159f, 3.14159f, "%.3f rad");
                        ImGui::DragFloat3("Scale", &model->transform.scale[0], 0.1f, 0.01f, 100.0f, "%.3f");
                    }
                    ImGui::TreePop(); // Finaliza el nodo del Model
                }
            }
            ImGui::TreePop(); // Finaliza el nodo del ModelContainer
        }
    }
    

    ImGui::End();
}

void ExampleDeferred::OnCloseOpenGL()
{
}

void ExampleDeferred::LoadModelInScene(libCore::ImportModelData importModelData)
{
    modelsInScene.push_back(libCore::ModelLoader::LoadModel(importModelData));
    //modelsInScene.push_back(libCore::SkeletalModelLoader::LoadModel(importModelData));
}

