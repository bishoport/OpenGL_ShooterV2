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
	//--MANAGERS
	Scope<GuiLayer> guiLayer = nullptr; //esto se declara en el cpp porque si se pone en el h, hay errores de includes c�clicos
	//Scope<ViewportManager> viewportManager = nullptr; //esto se declara en el cpp porque si se pone en el h, hay errores de includes c�clicos
	//Scope<RoofGenerator> roofGenerator = nullptr;
	// -------------------------------------------------
	// -------------------------------------------------
	
	// Size callback function
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		EventManager::OnWindowResizeEvent().trigger(width, height);
	}
	// -------------------------------------------------
	// -------------------------------------------------


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
	// -------------------------------------------------
	// -------------------------------------------------


	//--INIT & LIFE CYCLE
	bool EngineOpenGL::InitializeEngine(const std::string& windowTitle,
		int initialWindowWidth,
		int initialWindowHeight)
	{
		windowWidth = initialWindowWidth;
		windowHeight = initialWindowHeight;


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
		//libCore::ShaderManager shaderManager;
		std::string shadersDirectory = "assets/shaders/";
		//shaderManager.setShaderDataLoad("basic",        shadersDirectory + "basic.vert",    shadersDirectory + "basic.frag");
		ShaderManager::setShaderDataLoad("colorQuadFBO", shadersDirectory + "quad_fbo.vert", shadersDirectory + "color_quad_fbo.frag");
		
		//-DEBUG & TEXT
		ShaderManager::setShaderDataLoad("debug", shadersDirectory + "debug.vert", shadersDirectory + "debug.frag");
		ShaderManager::setShaderDataLoad("text",  shadersDirectory + "text.vert",  shadersDirectory + "text.frag");

		//-SSAO
		ShaderManager::setShaderDataLoad("ssao",     shadersDirectory + "ssao.vert", shadersDirectory + "ssao.frag");
		ShaderManager::setShaderDataLoad("ssaoBlur", shadersDirectory + "ssao.vert", shadersDirectory + "ssao_blur.frag");

		//-SKYBOX
		ShaderManager::setShaderDataLoad("dynamicSkybox", shadersDirectory + "skybox/dynamicSkybox.vs", shadersDirectory + "skybox/dynamicSkybox.fs");

		//-G_Buffer
		ShaderManager::setShaderDataLoad("geometryPass", shadersDirectory + "deferred/geometryPass.vert", shadersDirectory + "deferred/geometryPass.frag");
		ShaderManager::setShaderDataLoad("lightingPass", shadersDirectory + "deferred/lightingPass.vert", shadersDirectory + "deferred/lightingPass.frag");
		ShaderManager::setShaderDataLoad("combinePass",  shadersDirectory + "combine.vert",               shadersDirectory + "combine.frag");

		//-IBL
		ShaderManager::setShaderDataLoad("equirectangularToCubemap", shadersDirectory + "IBL/cubemap.vs"             , shadersDirectory + "IBL/equirectangular_to_cubemap.fs");
		ShaderManager::setShaderDataLoad("irradiance",               shadersDirectory + "IBL/cubemap.vs"             , shadersDirectory + "IBL/irradiance_convolution.fs");
		ShaderManager::setShaderDataLoad("prefilter",                shadersDirectory + "IBL/cubemap.vs"             , shadersDirectory + "IBL/prefilter.fs");
		ShaderManager::setShaderDataLoad("brdf",                     shadersDirectory + "IBL/brdf.vs"                , shadersDirectory + "IBL/brdf.fs");
		ShaderManager::setShaderDataLoad("background",               shadersDirectory + "IBL/background.vs"          , shadersDirectory + "IBL/background.fs");
		ShaderManager::setShaderDataLoad("captureIBL",               shadersDirectory + "IBL/capture_enviroment.vert", shadersDirectory + "IBL/capture_enviroment.frag");

		//-SHADOWS
		ShaderManager::setShaderDataLoad("direct_light_depth_shadows", shadersDirectory + "shadows/directLight_shadow_mapping_depth_shader.vs", shadersDirectory + "shadows/directLight_shadow_mapping_depth_shader.fs");

		//-STENCIL MOUSE PICKING
		ShaderManager::setShaderDataLoad("stencil", shadersDirectory + "Stencil.vert", shadersDirectory + "Stencil.frag");
		
		ShaderManager::LoadAllShaders();
		//-----------------------------------------------------------------


		// -- IMGUI
		if (useImGUI == true)
		{
			guiLayer = CreateScope<GuiLayer>(window, windowWidth, windowHeight);
			guiLayer->SetCallBackFunc([this](const std::vector<Vector2d>& points, const std::vector<Vector2d>& holePoints) {
				this->CreateRoof(points, holePoints);
				});
			//g_imGUILoopFnc = imGUILoopFnc;
		}
		// -------------------------------------------------

		// --InputManager
		//InputManager::Instance().subscribe();
		//---------------------------------------------------------------------------

		// -- ViewportManager
		//viewportManager = CreateScope<ViewportManager>();
		//---------------------------------------------------------------------------
		 		
		//-- FREETYPE
		FreeTypeManager::GetInstance().init();
		//---------------------------------------------------------------------------

		// -- ASSETS MANAGER (load Default Assets)
		AssetsManager::GetInstance().LoadDefaultAssets();
		//---------------------------------------------------------------------------

		//-- ROOF GENERATOR
		//roofGenerator = CreateScope<RoofGenerator>();
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
			UpdateBeforeRender();
			//-------------------------------------------
			

			//--MAIN LOOP FUNCTION CALL
			RenderViewports();
			// -------------------------------------------

			//-- ImGUI
			if (useImGUI)
			{
				// En tu bucle principal o en el manejo de eventos
				if (ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
				{
					mouseInImGUI = true;
				}
				else
				{
					mouseInImGUI = false;
				}

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

		//if (g_closeGlFnc)
		//{
		//	g_closeGlFnc();
		//}

		glfwTerminate();
	}
	// -------------------------------------------------
	// -------------------------------------------------


	//--VIEWPORTS
	void EngineOpenGL::CreateViewport(std::string name, glm::vec3 cameraPosition, CAMERA_CONTROLLERS controller)
	{
		ViewportManager::GetInstance().CreateViewport(name, cameraPosition, windowWidth, windowHeight, controller);
	}
	void EngineOpenGL::RenderViewports()
	{
		//RENDERING
		Renderer::getInstance().RenderViewport(ViewportManager::GetInstance().viewports[currentViewport], m_deltaTime, modelsInScene);
		Renderer::getInstance().ShowViewportInQuad(ViewportManager::GetInstance().viewports[currentViewport]);
		//-------------------------------------------
	}
	// -------------------------------------------------
	// -------------------------------------------------


	//--UPDATES
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
	void UpdateModelAABB(const Ref<libCore::Model>& model) {
		glm::mat4 modelMatrix = model->transform.getLocalModelMatrix();
		for (const auto& mesh : model->meshes) {
			mesh->UpdateAABB(modelMatrix);
		}
		// Llamada recursiva para los modelos hijos
		for (const auto& child : model->childs) {
			UpdateModelAABB(child);
		}
	}
	void UpdateModelTransform(const Ref<libCore::Model>& model) {
		model->transform.updateRotationFromEulerAngles();
		// Llamada recursiva para los modelos hijos
		for (const auto& child : model->childs) {
			UpdateModelTransform(child);
		}
	}
	void EngineOpenGL::checkRayModelIntersection(const Ref<libCore::Model>& model, const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::mat4& accumulatedTransform) {
		
		glm::mat4 modelMatrix = accumulatedTransform * model->transform.getLocalModelMatrix();

		for (const auto& mesh : model->meshes) {
			// Transformar AABB
			glm::vec3 transformedMin = glm::vec3(modelMatrix * glm::vec4(mesh->minBounds, 1.0));
			glm::vec3 transformedMax = glm::vec3(modelMatrix * glm::vec4(mesh->maxBounds, 1.0));

			// Verificar la intersecci�n del rayo con la AABB transformada
			if (rayIntersectsBoundingBox(rayOrigin, rayDirection, transformedMin, transformedMax)) 
			{
				modelsInRay.push_back(model);
				std::cout << "model " << model->name << std::endl;
			}
		}

		// Recorrer modelos hijos
		for (const auto& child : model->childs) {
			checkRayModelIntersection(child, rayOrigin, rayDirection, modelMatrix);
		}
	}
	
	void EngineOpenGL::UpdateBeforeRender()
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

		//--UPDATE MODEL TRANSFORM
		for (const auto& model : modelsInScene) {
			UpdateModelTransform(model);
		}
		//-------------------------------------------

		//--UPDATE AABB
		for (const auto& model : modelsInScene) {
			UpdateModelAABB(model);
		}
		//-------------------------------------------

		//--MOUSE PICKING
		if (mouseInImGUI == false && usingGizmo == false)
		{
			if (isSelectingObject == true)
			{
				return;
			}

			isSelectingObject = false;

			float mouseX, mouseY;
			std::tie(mouseX, mouseY) = InputManager::Instance().GetMousePosition();

			if (InputManager::Instance().IsMouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT) && isSelectingObject == false)
			{
				modelsInRay.clear();

				float normalizedX = (2.0f * mouseX) / ViewportManager::GetInstance().viewports[currentViewport]->viewportSize.x - 1.0f;
				float normalizedY = ((2.0f * mouseY) / ViewportManager::GetInstance().viewports[currentViewport]->viewportSize.y - 1.0f) * -1.0f;

				glm::vec3 clipSpaceCoordinates(normalizedX, normalizedY, -1.0);
				glm::vec4 homogenousClipCoordinates = glm::vec4(clipSpaceCoordinates, 1.0);
				glm::mat4 invProjView = glm::inverse(ViewportManager::GetInstance().viewports[currentViewport]->camera->cameraMatrix);
				glm::vec4 homogenousWorldCoordinates = invProjView * homogenousClipCoordinates;
				glm::vec3 worldCoordinates = glm::vec3(homogenousWorldCoordinates) / homogenousWorldCoordinates.w;

				glm::vec3 rayOrigin = ViewportManager::GetInstance().viewports[currentViewport]->camera->Position;
				glm::vec3 rayDirection = glm::normalize(worldCoordinates - rayOrigin);

				for (const auto& model : modelsInScene) {
					checkRayModelIntersection(model, rayOrigin, rayDirection, glm::mat4(1.0f));
				}

				if (modelsInRay.size() == 1) {
					currentSelectedModelInScene = modelsInRay[0];
					isSelectingObject = false; // No need to select, auto-selected
					showModelSelectionCombo = false;
				}
				else if (modelsInRay.size() > 1) {
					isSelectingObject = true; // Multiple options, need to select
					showModelSelectionCombo = true;
				}
				else {
					isSelectingObject = false; // No selection
					showModelSelectionCombo = false;
				}
			}
		}
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
		auto modelBuild = CreateRef<Model>();
		modelBuild->transform.position = pos;
		modelBuild->name = "PRIMITIVE_DOT";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateDot());
		

		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->albedoColor = polygonColor;

		material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
		//material->normalMap = assetsManager.GetTexture("default_normal");
		//material->metallicMap = assetsManager.GetTexture("default_metallic");
		//material->roughnessMap = assetsManager.GetTexture("default_roughness");
		//material->aOMap = assetsManager.GetTexture("default_ao");

		modelBuild->materials.push_back(material);
		modelsInScene.push_back(modelBuild);
	}
	void EngineOpenGL::CreatePrefabLine(const glm::vec3& point1, const glm::vec3& point2)
	{
		auto modelBuild = CreateRef<Model>();

		modelBuild->name = "PRIMITIVE_LINE";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateLine(point1, point2));


		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->albedoColor.r = 1.0f;
		material->albedoColor.g = 1.0f;
		material->albedoColor.b = 1.0f;

		material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
		material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
		material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
		material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
		material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelsInScene.push_back(modelBuild);
	}
	void EngineOpenGL::CreateTriangle(const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3)
	{
		auto modelBuild = CreateRef<Model>();


		modelBuild->name = "PRIMIVITE_TRIANGLE";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateTriangle(pos1, pos2, pos3));


		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->albedoColor.r = 1.0f;
		material->albedoColor.g = 1.0f;
		material->albedoColor.b = 1.0f;

		material->albedoMap = AssetsManager::GetInstance().GetTexture("checker");
		material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
		material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
		material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
		material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelsInScene.push_back(modelBuild);
	}
	void EngineOpenGL::CreatePrefabSphere(float radius, unsigned int sectorCount, unsigned int stackCount)
	{
		auto modelBuild = CreateRef<Model>();

		modelBuild->name = "PRIMIVITE_SPHERE";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateSphere(0.01f, 6, 6));

		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->albedoColor.r = 1.0f;
		material->albedoColor.g = 1.0f;
		material->albedoColor.b = 1.0f;

		material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
		material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
		material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
		material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
		material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelsInScene.push_back(modelBuild);
	}
	void EngineOpenGL::CreatePrefabCube(glm::vec3 position)
	{
		auto modelBuild = CreateRef<Model>();

		modelBuild->name = "PRIMIVITE_CUBE";
		modelBuild->meshes.push_back(PrimitivesHelper::CreateCube());
		modelBuild->transform.position = position;

		//--DEFAULT_MATERIAL
		auto material = CreateRef<Material>();
		material->materialName = "default_material";

		material->albedoColor.r = 1.0f;
		material->albedoColor.g = 1.0f;
		material->albedoColor.b = 1.0f;

		material->albedoMap = AssetsManager::GetInstance().GetTexture("default_albedo");
		material->normalMap = AssetsManager::GetInstance().GetTexture("default_normal");
		material->metallicMap = AssetsManager::GetInstance().GetTexture("default_metallic");
		material->roughnessMap = AssetsManager::GetInstance().GetTexture("default_roughness");
		material->aOMap = AssetsManager::GetInstance().GetTexture("default_ao");

		modelBuild->materials.push_back(material);

		modelsInScene.push_back(modelBuild);
	}
	void EngineOpenGL::CreateRoof(const std::vector<Vector2d>& points, const std::vector<Vector2d>& holes)
	{
		//modelsInScene.push_back(roofGenerator->GenerateRoof(points, holes));
	}
	// -------------------------------------------------
	// -------------------------------------------------


	//--IMGUI
	void EngineOpenGL::DrawHierarchyPanel()
	{
		if (useImGUI)
		{
			//--SELECT MODEL FROM RAY POPUP
			if (isSelectingObject == true)
			{
				ImGui::OpenPopup("Select Model");
			}
			if (ImGui::BeginPopup("Select Model"))
			{
				for (const auto& model : modelsInRay) {
					if (ImGui::Button(model->name.c_str())) {
						currentSelectedModelInScene = model;
						isSelectingObject = false; // Esta asignaci�n cerrar� el popup al finalizar el frame
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndPopup();
			}
			//--------------------------------------------------------

			//--CHECK ImGizmo
			guiLayer->checkGizmo(ViewportManager::GetInstance().viewports[currentViewport]);
			//--------------------------------------------------------

			guiLayer->DrawHierarchyPanel(modelsInScene);
			guiLayer->DrawInspectorPanel(currentSelectedModelInScene);
			guiLayer->DrawLightsPanel(LightsManager::GetLights());
			guiLayer->DrawMaterialsPanel();
			//guiLayer->RenderCheckerMatrix(); //Panel para el editor de roofs

			Renderer::getInstance().ShowControlsGUI();
			ViewportManager::GetInstance().DrawPanelGUI();

			
			//--------------------------------------------------------
		}
	}
	// -------------------------------------------------
	// -------------------------------------------------
}
