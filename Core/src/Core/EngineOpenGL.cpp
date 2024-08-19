#include "EngineOpenGL.h"
#include "../input/InputManager.h"
#include "../GUI/GuiLayer.h"
#include "ViewportManager.hpp"
#include "Renderer.hpp"
#include "../tools/RoofGenerator.hpp"
#include "../tools/LightsManager.hpp"
#include "../tools/SkyBox.hpp"
#include "../ECS/EntityManager.hpp"
#include "../tools/AssetsManager.h"
#include "../tools/FreeTypeManager.h"



namespace libCore
{
	//--MANAGERS
	Scope<GuiLayer> guiLayer = nullptr; //esto se declara en el cpp porque si se pone en el h, hay errores de includes cíclicos
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


		// -- ASSETS MANAGER (load Default Assets)
		AssetsManager::GetInstance().LoadDefaultAssets();
		AssetsManager::GetInstance().CreateDefaultMaterial();
		//---------------------------------------------------------------------------


		// -- IMGUI
		guiLayer = CreateScope<GuiLayer>(window, windowWidth, windowHeight);
		// -------------------------------------------------

		// -- SCENE
		currentScene = CreateScope<Scene>();
		// -------------------------------------------------
		

		//-- FREETYPE
		FreeTypeManager::GetInstance().init();
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
	void EngineOpenGL::InitializeMainLoop()
	{
		Timestep lastFrameTime = static_cast<float>(glfwGetTime());

		running = true;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window) && running)
		{
			//DELTA TIME
			Timestep currentFrameTime = static_cast<float>(glfwGetTime());
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
			if (ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
			{
				mouseInImGUI = true;
			}
			else
			{
				mouseInImGUI = false;
			}
			guiLayer->Draw();
			// -------------------------------------------


			//END INPUT UPDATE
			InputManager::Instance().EndFrame();

			// Actualiza la ventana
			glfwPollEvents();

			// Intercambia los buffers
			glfwSwapBuffers(window);
		}
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
		Renderer::getInstance().RenderViewport(ViewportManager::GetInstance().viewports[currentViewport], m_deltaTime);
		Renderer::getInstance().ShowViewportInQuad(ViewportManager::GetInstance().viewports[currentViewport]);
		//-------------------------------------------
	}
	// -------------------------------------------------
	// -------------------------------------------------

	//--ENGINE CONTROL STATES
	void libCore::EngineOpenGL::ChangeEngineState(EngineStates newState)
	{
		EngineOpenGL::GetInstance().engineState = newState;

		if (EngineOpenGL::GetInstance().engineState == EDITOR)
		{

		}
		else if (EngineOpenGL::GetInstance().engineState == EDITOR_PLAY)
		{
			EntityManager::GetInstance().InitScripts();
		}
		else if (EngineOpenGL::GetInstance().engineState == PLAY)
		{
			EntityManager::GetInstance().InitScripts();
		}

	}
	// -------------------------------------------------
	// -------------------------------------------------



	//--UPDATES
	void EngineOpenGL::UpdateBeforeRender()
	{


		//--INPUT UPDATE
		InputManager::Instance().Update();

		if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_F1)) //Editor
		{
			ChangeEngineState(EngineStates::EDITOR);
		}
		else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_F2)) //Editor Play
		{
			ChangeEngineState(EngineStates::EDITOR_PLAY);
		}
		else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_F3)) //Play
		{
			ChangeEngineState(EngineStates::PLAY);
		}
		else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_ESCAPE)) //BACK TO EDITOR MODE WITH ESC FROM PLAY MODE
		{
			ChangeEngineState(EngineStates::EDITOR);
		}
		else if (InputManager::Instance().IsKeyPressed(GLFW_KEY_LEFT_CONTROL) || InputManager::Instance().IsKeyPressed(GLFW_KEY_RIGHT_CONTROL))
		{
			if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_C)) // Ctrl + C
			{
				EntityManager::GetInstance().DuplicateEntity();
			}
		}

		//--ImGizmo
		else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_T))
		{
			guiLayer->m_GizmoOperation = GizmoOperation::Translate;
		}
		else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_Y))
		{
			guiLayer->m_GizmoOperation = GizmoOperation::Rotate3D;
		}
		else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_U))
		{
			guiLayer->m_GizmoOperation = GizmoOperation::Scale;
		}
		//------------------------------------------------------------------------------
		//-------------------------------------------------------------------





		//--DIRECTIONAL LIGHT UPDATE
		if (LightsManager::GetInstance().GetDirectionalLight() != nullptr)
		{
			LightsManager::GetInstance().GetDirectionalLight()->UpdateLightPosition();
		}
		//-------------------------------------------------------------------

		//--UPDATE ENTITIES WITH TRANSFORM & AABB
		EntityManager::GetInstance().UpdateGameObjects(m_deltaTime);
		//-------------------------------------------
		
		//--UPDATE GAME CAMERA IN VIEWPORT
		entt::entity mainCameraEntity = EntityManager::GetInstance().GetEntityByName("MainCamera");

		if (mainCameraEntity != entt::null)
		{
			auto& cameraComponent    = EntityManager::GetInstance().GetComponent<CameraComponent>(mainCameraEntity);
			auto& transformComponent = EntityManager::GetInstance().GetComponent<TransformComponent>(mainCameraEntity);

			if (cameraComponent.camera == nullptr)
			{
				cameraComponent.camera = ViewportManager::GetInstance().viewports[0]->gameCamera;
			}

			ViewportManager::GetInstance().viewports[currentViewport]->gameCamera->Position = transformComponent.transform->GetPosition();
		}

		if (engineState == EDITOR )
		{
			ViewportManager::GetInstance().viewports[currentViewport]->camera = ViewportManager::GetInstance().viewports[currentViewport]->editorCamera;
		}
		else if (engineState == PLAY || engineState == EDITOR_PLAY)
		{
			ViewportManager::GetInstance().viewports[currentViewport]->camera = ViewportManager::GetInstance().viewports[currentViewport]->gameCamera;
		}
		


		if (engineState == EDITOR || engineState == EDITOR_PLAY)
		{
			//--MOUSE PICKING
			if (mouseInImGUI == false && usingGizmo == false)
			{
				if (guiLayer->isSelectingObject == true)
				{
					return;
				}

				guiLayer->isSelectingObject = false;

				float mouseX, mouseY;
				std::tie(mouseX, mouseY) = InputManager::Instance().GetMousePosition();

				if (InputManager::Instance().IsMouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT) && guiLayer->isSelectingObject == false)
				{
					EntityManager::GetInstance().entitiesInRay.clear();

					float normalizedX = (2.0f * mouseX) / ViewportManager::GetInstance().viewports[currentViewport]->viewportSize.x - 1.0f;
					float normalizedY = ((2.0f * mouseY) / ViewportManager::GetInstance().viewports[currentViewport]->viewportSize.y - 1.0f) * -1.0f;

					glm::vec3 clipSpaceCoordinates(normalizedX, normalizedY, -1.0);
					glm::vec4 homogenousClipCoordinates = glm::vec4(clipSpaceCoordinates, 1.0);
					glm::mat4 invProjView = glm::inverse(ViewportManager::GetInstance().viewports[currentViewport]->camera->cameraMatrix);
					glm::vec4 homogenousWorldCoordinates = invProjView * homogenousClipCoordinates;
					glm::vec3 worldCoordinates = glm::vec3(homogenousWorldCoordinates) / homogenousWorldCoordinates.w;

					glm::vec3 rayOrigin = ViewportManager::GetInstance().viewports[currentViewport]->camera->Position;
					glm::vec3 rayDirection = glm::normalize(worldCoordinates - rayOrigin);

					EntityManager::GetInstance().CheckRayModelIntersection(rayOrigin, rayDirection);

					if (EntityManager::GetInstance().entitiesInRay.size() == 1) 
					{
						EntityManager::GetInstance().currentSelectedEntityInScene = EntityManager::GetInstance().entitiesInRay[0];
						guiLayer->isSelectingObject = false; // No need to select, auto-selected
						guiLayer->showModelSelectionCombo = false;
					}
					else if (EntityManager::GetInstance().entitiesInRay.size() > 1) {
						guiLayer->isSelectingObject = true; // Multiple options, need to select
						guiLayer->showModelSelectionCombo = true;
						EntityManager::GetInstance().currentSelectedEntityInScene = entt::null;
					}
					else {
						guiLayer->isSelectingObject = false; // No selection
						guiLayer->showModelSelectionCombo = false;
						EntityManager::GetInstance().currentSelectedEntityInScene = entt::null;
						EntityManager::GetInstance().entitiesInRay.clear();
					}
				}
			}
		}
		//-------------------------------------------
	}
	// -------------------------------------------------
	// -------------------------------------------------
}