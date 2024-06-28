#include "EngineOpenGL.h"
#include "../input/InputManager.h"
#include "GuiLayer.h"
#include "ViewportManager.hpp"
#include "Renderer.hpp"
#include "../tools/RoofGenerator.hpp"
#include "../tools/LightsManager.hpp"
#include "../tools/SkyBox.hpp"


namespace libCore
{
	//MANAGERS
	Scope<GuiLayer> guiLayer = nullptr; //esto se declara en el cpp porque si se pone en el h, hay errores de includes cíclicos
	Scope<ViewportManager> viewportManager = nullptr; //esto se declara en el cpp porque si se pone en el h, hay errores de includes cíclicos
	Scope<RoofGenerator> roofGenerator = nullptr;
	Scope<Renderer> renderer = nullptr;
	

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		EventManager::OnWindowResizeEvent().trigger(width, height);
	}

	// Message callback function
	void GLAPIENTRY MessageCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		// Print the message based on the severity
		if (severity == GL_DEBUG_SEVERITY_HIGH) {
			std::cerr << "GL HIGH SEVERITY: " << message << std::endl;
		}
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
			std::cerr << "GL MEDIUM SEVERITY: " << message << std::endl;
		}
		else if (severity == GL_DEBUG_SEVERITY_LOW) {
			std::cerr << "GL LOW SEVERITY: " << message << std::endl;
		}
		else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
			std::cout << "GL NOTIFICATION: " << message << std::endl;
		}
	}


	//--INIT & LIFE CYCLE
	bool EngineOpenGL::InitializeEngine(const std::string& windowTitle,
		int initialWindowWidth,
		int initialWindowHeight,
		MainLoopFnc mainLoodFnc,
		CloseGlFnc closeGlFnc,
		ImGUILoopFnc imGUILoopFnc)
	{
		windowWidth = initialWindowWidth;
		windowHeight = initialWindowHeight;

		g_mainLoodFnc = mainLoodFnc;
		g_closeGlFnc = closeGlFnc;

		/* Initialize the library */
		if (!glfwInit())
		{
			return false;
		}

		//--GLFW: initialize and configure 
		glfwInit();
		const char* glsl_version = "#version 460";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
		// ------------------------------


		//--GLFW window creation
		window = glfwCreateWindow(initialWindowWidth, initialWindowHeight, windowTitle.c_str(), NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window);
		SetupInputCallbacks();
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		//-------------------------------------------------


		//--Glad: load all OpenGL function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}
		//-------------------------------------------------


		//--GL debug output
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(MessageCallback, 0);

		// Filter out notification messages
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

		// Optionally, you can further filter by source and type
		// Example: Ignore performance warnings
		// glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_PERFORMANCE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);

		// Example: Ignore all messages except for high severity errors
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);

		
		//-----------------------------


		// -- OPENGL FLAGS
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glEnable(GL_MULTISAMPLE); // enabled by default on some drivers, but not all so always enable to make sure
		//-----------------------------

		// -- SHADERS
		std::string shadersDirectory = "assets/shaders/";
		shaderManager.setShaderDataLoad("basic",        shadersDirectory + "basic.vert",    shadersDirectory + "basic.frag");
		shaderManager.setShaderDataLoad("colorQuadFBO", shadersDirectory + "quad_fbo.vert", shadersDirectory + "color_quad_fbo.frag");
		
		//-DEBUG & TEXT
		shaderManager.setShaderDataLoad("debug", shadersDirectory + "debug.vert", shadersDirectory + "debug.frag");
		shaderManager.setShaderDataLoad("text",  shadersDirectory + "text.vert",  shadersDirectory + "text.frag");

		//-SSAO
		shaderManager.setShaderDataLoad("ssao",     shadersDirectory + "ssao.vert", shadersDirectory + "ssao.frag");
		shaderManager.setShaderDataLoad("ssaoBlur", shadersDirectory + "ssao.vert", shadersDirectory + "ssao_blur.frag");

		//-SKYBOX
		shaderManager.setShaderDataLoad("dynamicSkybox", shadersDirectory + "skybox/dynamicSkybox.vs", shadersDirectory + "skybox/dynamicSkybox.fs");

		//-G_Buffer
		shaderManager.setShaderDataLoad("geometryPass", shadersDirectory + "deferred/geometryPass.vert", shadersDirectory + "deferred/geometryPass.frag");
		shaderManager.setShaderDataLoad("lightingPass", shadersDirectory + "deferred/lightingPass.vert", shadersDirectory + "deferred/lightingPass.frag");
		shaderManager.setShaderDataLoad("combinePass",  shadersDirectory + "combine.vert",               shadersDirectory + "combine.frag");

		//-IBL
		shaderManager.setShaderDataLoad("equirectangularToCubemap", shadersDirectory + "IBL/cubemap.vs"   , shadersDirectory + "IBL/equirectangular_to_cubemap.fs");
		shaderManager.setShaderDataLoad("irradiance",               shadersDirectory + "IBL/cubemap.vs"   , shadersDirectory + "IBL/irradiance_convolution.fs");
		shaderManager.setShaderDataLoad("prefilter",                shadersDirectory + "IBL/cubemap.vs"   , shadersDirectory + "IBL/prefilter.fs");
		shaderManager.setShaderDataLoad("brdf",                     shadersDirectory + "IBL/brdf.vs"      , shadersDirectory + "IBL/brdf.fs");
		shaderManager.setShaderDataLoad("background",               shadersDirectory + "IBL/background.vs", shadersDirectory + "IBL/background.fs");

		//-SHADOWS
		shaderManager.setShaderDataLoad("direct_light_depth_shadows", shadersDirectory + "shadows/directLight_shadow_mapping_depth_shader.vs", shadersDirectory + "shadows/directLight_shadow_mapping_depth_shader.fs");

		shaderManager.LoadAllShaders();
		//-----------------------------------------------------------------


		// -- IMGUI
		if (imGUILoopFnc && useImGUI == true)
		{
			guiLayer = CreateScope<GuiLayer>(window, windowWidth, windowHeight);
			guiLayer->SetCallBackFunc([this](const std::vector<Vector2d>& points, const std::vector<Vector2d>& holePoints) {
				this->CreateRoof(points, holePoints);
				});
			g_imGUILoopFnc = imGUILoopFnc;
		}
		// -------------------------------------------------

		// --InputManager
		//InputManager::Instance().subscribe();

		// -- ViewportManager
		viewportManager = CreateScope<ViewportManager>();
		//---------------------------------------------------------------------------
		 		
		//-- FREETYPE
		freeTypeManager = new libCore::FreeTypeManager();
		//---------------------------------------------------------------------------

		// -- ASSETS MANAGER (load Default Assets)
		assetsManager.LoadDefaultAssets();
		//---------------------------------------------------------------------------

		//-- ROOF GENERATOR
		roofGenerator = CreateScope<RoofGenerator>();
		//---------------------------------------------------------------------------

		// -- RENDERER
		renderer = CreateScope<Renderer>();
		//---------------------------------------------------------------------------

		return true;
	}
	void EngineOpenGL::SetupInputCallbacks()
	{
		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			if (action == GLFW_PRESS)
			{
				InputManager::Instance().SetKeyPressed(key, true);
			}
			else if (action == GLFW_RELEASE) {
				InputManager::Instance().SetKeyPressed(key, false);
			}
			});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
			double x, y;
			glfwGetCursorPos(window, &x, &y);

			if (action == GLFW_PRESS) {
				InputManager::Instance().SetMousePressed(button, x, y, true);
			}
			else if (action == GLFW_RELEASE) {
				InputManager::Instance().SetMousePressed(button, x, y, false);
			}
			});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
			InputManager::Instance().SetMousePosition(xpos, ypos);
			});
	}
	void EngineOpenGL::StopMainLoop()
	{
		running = false;
	}
	void EngineOpenGL::begin()
	{
		if (useImGUI)
		{
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}
	}
	void EngineOpenGL::InitializeMainLoop()
	{
		Timestep lastFrameTime = glfwGetTime();

		running = true;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window) && running)
		{
			//DELTA TIME
			Timestep currentFrameTime = glfwGetTime();
			m_deltaTime = currentFrameTime - lastFrameTime;
			lastFrameTime = currentFrameTime;
			//-------------------------------------------
			
			//--WINDOW SIZE
			GLint windowWidth, windowHeight;
			glfwGetWindowSize(window, &windowWidth, &windowHeight);
			//-------------------------------------------


			//--START INPUT UPDATE
			InputManager::Instance().Update();
			if (LightsManager::GetInstance().GetDirectionalLight() != nullptr)
			{
				LightsManager::GetInstance().GetDirectionalLight()->UpdateLightPosition();
			}
			//-------------------------------------------
 

			//--MAIN INPUTS
			if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_F1))
			{
				useImGUI = !useImGUI;
			}
			if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_1))
			{
				currentViewport = 0;
			}
			else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_2))
			{
				currentViewport = 1;
			}
			//-------------------------------------------


			//--MAIN LOOP FUNCTION CALL
			RenderViewports();
			// -------------------------------------------

			//-- ImGUI
			if (useImGUI)
			{
				guiLayer->begin();
				guiLayer->renderMainMenuBar();
				guiLayer->renderDockers();
				DrawHierarchyPanel();
				guiLayer->end();
			}
			// -------------------------------------------

			//END INPUT UPDATE
			InputManager::Instance().EndFrame();

			// Actualiza la ventana
			glfwPollEvents();

			// Intercambia los buffers
			glfwSwapBuffers(window);
		}

		if (g_closeGlFnc)
		{
			g_closeGlFnc();
		}

		glfwTerminate();
	}
	// -------------------------------------------------
	// -------------------------------------------------


	//--VIEWPORTS
	void EngineOpenGL::CreateViewport(std::string name, glm::vec3 cameraPosition, CAMERA_CONTROLLERS controller)
	{
		viewportManager->CreateViewport(name, cameraPosition, windowWidth, windowHeight, controller);
	}
	void EngineOpenGL::RenderViewports()
	{
		renderer->RenderViewport(viewportManager->viewports[currentViewport], m_deltaTime, modelsInScene);
		renderer->ShowViewportInQuad(viewportManager->viewports[currentViewport]);
	}
	// -------------------------------------------------
	// -------------------------------------------------


	//--CREADOR DE PREFABS
	void EngineOpenGL::CreatePrefabExternalModel(ImportModelData importModelData)
	{
		modelsInScene.push_back(libCore::ModelLoader::LoadModel(importModelData));
	}
	void EngineOpenGL::CreatePrefabDot(const glm::vec3& pos, const glm::vec3& polygonColor)
	{
		auto modelContainer = CreateRef<ModelContainer>();
		modelContainer->skeletal = false;

		auto modelBuild = CreateRef<Model>();
		modelBuild->transform.position = pos;
		modelContainer->name = "PRIMITIVE_DOT";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateDot());
		

		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->albedoColor = polygonColor;

		material->albedoMap = assetsManager.GetTexture("default_albedo");
		//material->normalMap = assetsManager.GetTexture("default_normal");
		//material->metallicMap = assetsManager.GetTexture("default_metallic");
		//material->roughnessMap = assetsManager.GetTexture("default_roughness");
		//material->aOMap = assetsManager.GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelContainer->models.push_back(modelBuild);

		modelsInScene.push_back(modelContainer);
	}
	void EngineOpenGL::CreatePrefabLine(const glm::vec3& point1, const glm::vec3& point2)
	{
		auto modelContainer = CreateRef<ModelContainer>();
		modelContainer->skeletal = false;

		auto modelBuild = CreateRef<Model>();

		modelContainer->name = "PRIMITIVE_LINE";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateLine(point1, point2));
		modelContainer->models.push_back(modelBuild);


		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->albedoColor.r = 1.0f;
		material->albedoColor.g = 1.0f;
		material->albedoColor.b = 1.0f;

		material->albedoMap = assetsManager.GetTexture("default_albedo");
		material->normalMap = assetsManager.GetTexture("default_normal");
		material->metallicMap = assetsManager.GetTexture("default_metallic");
		material->roughnessMap = assetsManager.GetTexture("default_roughness");
		material->aOMap = assetsManager.GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelsInScene.push_back(modelContainer);
	}
	void EngineOpenGL::CreateTriangle(const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3)
	{
		auto modelContainer = CreateRef<ModelContainer>();
		modelContainer->skeletal = false;

		auto modelBuild = CreateRef<Model>();


		modelContainer->name = "PRIMIVITE_TRIANGLE";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateTriangle(pos1, pos2, pos3));
		modelContainer->models.push_back(modelBuild);


		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->albedoColor.r = 1.0f;
		material->albedoColor.g = 1.0f;
		material->albedoColor.b = 1.0f;

		material->albedoMap = assetsManager.GetTexture("checker");
		material->normalMap = assetsManager.GetTexture("default_normal");
		material->metallicMap = assetsManager.GetTexture("default_metallic");
		material->roughnessMap = assetsManager.GetTexture("default_roughness");
		material->aOMap = assetsManager.GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelsInScene.push_back(modelContainer);
	}
	void EngineOpenGL::CreatePrefabSphere(float radius, unsigned int sectorCount, unsigned int stackCount)
	{
		auto modelContainer = CreateRef<ModelContainer>();
		modelContainer->skeletal = false;

		auto modelBuild = CreateRef<Model>();


		modelContainer->name = "PRIMIVITE_SPHERE";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateSphere(0.01f, 6, 6));
		modelContainer->models.push_back(modelBuild);


		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->albedoColor.r = 1.0f;
		material->albedoColor.g = 1.0f;
		material->albedoColor.b = 1.0f;

		material->albedoMap = assetsManager.GetTexture("default_albedo");
		material->normalMap = assetsManager.GetTexture("default_normal");
		material->metallicMap = assetsManager.GetTexture("default_metallic");
		material->roughnessMap = assetsManager.GetTexture("default_roughness");
		material->aOMap = assetsManager.GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelsInScene.push_back(modelContainer);
	}
	void EngineOpenGL::CreatePrefabCube()
	{
		auto modelContainer = CreateRef<ModelContainer>();
		modelContainer->skeletal = false;

		auto modelBuild = CreateRef<Model>();

		modelContainer->name = "PRIMIVITE_CUBE";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateCube());
		

		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->albedoColor.r = 1.0f;
		material->albedoColor.g = 1.0f;
		material->albedoColor.b = 1.0f;

		material->albedoMap = assetsManager.GetTexture("default_albedo");
		material->normalMap = assetsManager.GetTexture("default_normal");
		material->metallicMap = assetsManager.GetTexture("default_metallic");
		material->roughnessMap = assetsManager.GetTexture("default_roughness");
		material->aOMap = assetsManager.GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelContainer->models.push_back(modelBuild);

		modelsInScene.push_back(modelContainer);
	}
	void EngineOpenGL::CreateRoof(const std::vector<Vector2d>& points, const std::vector<Vector2d>& holes)
	{
		modelsInScene.push_back(roofGenerator->GenerateRoof(points, holes));
	}
	// -------------------------------------------------
	// -------------------------------------------------


	//--PANELS
	void EngineOpenGL::DrawHierarchyPanel()
	{
		if (useImGUI)
		{
			guiLayer->DrawHierarchyPanel(modelsInScene);
			guiLayer->DrawLightsPanel(LightsManager::GetLights());
			renderer->ShowControlsGUI();
			viewportManager->DrawPanelGUI();
			//guiLayer->RenderCheckerMatrix(); //Panel para el editor de roofs
		}
	}
	// -------------------------------------------------
	// -------------------------------------------------
}