#include "EngineOpenGL.h"
#include "../input/InputManager.h"
#include "GuiLayer.h"
#include "ViewportManager.hpp"
#include "Renderer.hpp"
#include "../tools/RoofGenerator.hpp"
#include "../tools/LightsManager.hpp"
#include "../tools/SkyBox.hpp"
#include "../tools/MousePicker.hpp"



namespace libCore
{
	//MANAGERS
	Scope<GuiLayer> guiLayer = nullptr; //esto se declara en el cpp porque si se pone en el h, hay errores de includes cíclicos
	Scope<ViewportManager> viewportManager = nullptr; //esto se declara en el cpp porque si se pone en el h, hay errores de includes cíclicos
	Scope<RoofGenerator> roofGenerator = nullptr;
	//Scope<Renderer> renderer = nullptr;
	

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
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_PERFORMANCE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);

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
		//shaderManager.setShaderDataLoad("basic",        shadersDirectory + "basic.vert",    shadersDirectory + "basic.frag");
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
		shaderManager.setShaderDataLoad("equirectangularToCubemap", shadersDirectory + "IBL/cubemap.vs"             , shadersDirectory + "IBL/equirectangular_to_cubemap.fs");
		shaderManager.setShaderDataLoad("irradiance",               shadersDirectory + "IBL/cubemap.vs"             , shadersDirectory + "IBL/irradiance_convolution.fs");
		shaderManager.setShaderDataLoad("prefilter",                shadersDirectory + "IBL/cubemap.vs"             , shadersDirectory + "IBL/prefilter.fs");
		shaderManager.setShaderDataLoad("brdf",                     shadersDirectory + "IBL/brdf.vs"                , shadersDirectory + "IBL/brdf.fs");
		shaderManager.setShaderDataLoad("background",               shadersDirectory + "IBL/background.vs"          , shadersDirectory + "IBL/background.fs");
		shaderManager.setShaderDataLoad("captureIBL",               shadersDirectory + "IBL/capture_enviroment.vert", shadersDirectory + "IBL/capture_enviroment.frag");

		//-SHADOWS
		shaderManager.setShaderDataLoad("direct_light_depth_shadows", shadersDirectory + "shadows/directLight_shadow_mapping_depth_shader.vs", shadersDirectory + "shadows/directLight_shadow_mapping_depth_shader.fs");

		//-STENCIL MOUSE PICKING
		shaderManager.setShaderDataLoad("stencil", shadersDirectory + "Stencil.vert", shadersDirectory + "Stencil.frag");
		
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
		//---------------------------------------------------------------------------

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
		Renderer::getInstance().initialize();
		//---------------------------------------------------------------------------

		// -- MOUSE PICKER
		//MousePicker::getInstance().initialize(window);
		//MousePicker::getInstance().setupStencilBuffer();
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

			//--UPDATES
			UpdateAfterRender();
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

	bool rayIntersectsBoundingBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3 boxMin, glm::vec3 boxMax)
	{
		float tMin = (boxMin.x - rayOrigin.x) / rayDirection.x;
		float tMax = (boxMax.x - rayOrigin.x) / rayDirection.x;

		if (tMin > tMax) std::swap(tMin, tMax);

		float tyMin = (boxMin.y - rayOrigin.y) / rayDirection.y;
		float tyMax = (boxMax.y - rayOrigin.y) / rayDirection.y;

		if (tyMin > tyMax) std::swap(tyMin, tyMax);

		if ((tMin > tyMax) || (tyMin > tMax))
			return false;

		if (tyMin > tMin)
			tMin = tyMin;

		if (tyMax < tMax)
			tMax = tyMax;

		float tzMin = (boxMin.z - rayOrigin.z) / rayDirection.z;
		float tzMax = (boxMax.z - rayOrigin.z) / rayDirection.z;

		if (tzMin > tzMax) std::swap(tzMin, tzMax);

		if ((tMin > tzMax) || (tzMin > tMax))
			return false;

		if (tzMin > tMin)
			tMin = tzMin;

		if (tzMax < tMax)
			tMax = tzMax;

		return true;
	}


	//--VIEWPORTS
	void EngineOpenGL::CreateViewport(std::string name, glm::vec3 cameraPosition, CAMERA_CONTROLLERS controller)
	{
		viewportManager->CreateViewport(name, cameraPosition, windowWidth, windowHeight, controller);
	}
	void EngineOpenGL::UpdateAfterRender()
	{
		//--INPUT UPDATE
		InputManager::Instance().Update();

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
		//-------------------------------------------------------------------


		//--DIRECTIONAL LIGHT UPDATE
		if (LightsManager::GetInstance().GetDirectionalLight() != nullptr)
		{
			LightsManager::GetInstance().GetDirectionalLight()->UpdateLightPosition();
		}
		//-------------------------------------------------------------------


		//--UPDATE AABB
		for (const auto& modelContainer : modelsInScene) {
			for (const auto& model : modelContainer->models) {
				glm::mat4 modelMatrix = model->transform.getLocalModelMatrix();
				for (const auto& mesh : model->meshes) {
					mesh->UpdateAABB(modelMatrix);
				}
			}
		}
		//-------------------------------------------

		//--MOUSE PICKING
		float mouseX, mouseY;
		std::tie(mouseX, mouseY) = InputManager::Instance().GetMousePosition();

		// Aquí empieza el raycasting
		if (InputManager::Instance().IsMouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			// Llevar un punto 2D a un espacio 3D (mouse position -> escena)
			float normalizedX = (2.0f * mouseX) / viewportManager->viewports[currentViewport]->viewportSize.x - 1.0f;
			float normalizedY = ((2.0f * mouseY) / viewportManager->viewports[currentViewport]->viewportSize.y - 1.0f) * -1.0f;

			glm::vec3 clipSpaceCoordinates(normalizedX, normalizedY, -1.0); // -1.0 para proyectar hacia adelante

			glm::vec4 homogenousClipCoordinates = glm::vec4(clipSpaceCoordinates, 1.0);
			glm::mat4 invProjView = glm::inverse(viewportManager->viewports[currentViewport]->camera->cameraMatrix); // Invertir solo la cameraMatrix que ya es projection * view
			glm::vec4 homogenousWorldCoordinates = invProjView * homogenousClipCoordinates;
			glm::vec3 worldCoordinates = glm::vec3(homogenousWorldCoordinates) / homogenousWorldCoordinates.w;

			// Preparamos el rayo para lanzarlo desde la cámara hasta la posición del mouse ya convertido al espacio 3D
			glm::vec3 rayOrigin = viewportManager->viewports[currentViewport]->camera->Position;
			glm::vec3 rayDirection = glm::normalize(worldCoordinates - rayOrigin);
			float rayLength = 1000.0f;  // Puedes ajustar esto según la escala de tu escena
			glm::vec3 rayEnd = rayOrigin + rayDirection * rayLength;

			//std::cout << "Camera Position: " << glm::to_string(rayOrigin) << std::endl;
			//std::cout << "Camera Orientation: " << glm::to_string(viewportManager->viewports[currentViewport]->camera->Orientation) << std::endl;
			//std::cout << "Ray Origin: " << glm::to_string(rayOrigin) << std::endl;
			//std::cout << "Ray Direction: " << glm::to_string(rayDirection) << std::endl;
			//CreatePrefabLine(rayOrigin, rayEnd);  // Dibuja el rayo para visualización

			for (const auto& modelContainer : modelsInScene) {
				for (const auto& model : modelContainer->models) {
					for (const auto& mesh : model->meshes) {
						// Verificar la intersección del rayo con la AABB transformada
						if (rayIntersectsBoundingBox(rayOrigin, rayDirection, mesh->worldMinBounds, mesh->worldMaxBounds)) {
							std::cout << "MESH " << mesh->meshName << std::endl;
						}
					}
				}
			}
		}
		//-------------------------------------------
	}
	void EngineOpenGL::RenderViewports()
	{
		//RENDERING
		Renderer::getInstance().RenderViewport(viewportManager->viewports[currentViewport], m_deltaTime, modelsInScene);
		Renderer::getInstance().ShowViewportInQuad(viewportManager->viewports[currentViewport]);
		//-------------------------------------------
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
	void EngineOpenGL::CreatePrefabCube(glm::vec3 position)
	{
		auto modelContainer = CreateRef<ModelContainer>();
		modelContainer->skeletal = false;

		auto modelBuild = CreateRef<Model>();

		modelContainer->name = "PRIMIVITE_CUBE";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateCube());
		modelBuild->transform.position = position;

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
			guiLayer->DrawMaterialsPanel();
			Renderer::getInstance().ShowControlsGUI();
			viewportManager->DrawPanelGUI();
			//guiLayer->RenderCheckerMatrix(); //Panel para el editor de roofs
		}
	}
	// -------------------------------------------------
	// -------------------------------------------------
}
