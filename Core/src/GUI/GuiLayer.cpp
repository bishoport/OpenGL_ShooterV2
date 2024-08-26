#include "GuiLayer.h"

#include "../tools/ShaderManager.h"
#include <imGizmo/ImGuizmo.h>
#include <imgui_internal.h>

#include "../tools/LightsManager.hpp"
#include "../ECS/EntityManager.hpp"
#include "../Core/ViewportManager.hpp"
#include "../Core/Renderer.hpp"


namespace libCore
{
    //--CONSTRUCTOR
    GuiLayer::GuiLayer(GLFWwindow* window, float window_W, float window_H)
    {
        m_window_W = window_W;
        m_window_H = window_H;

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Enable Docking
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        //io.ConfigFlags  |= ImGuiConfigFlags_ViewportsEnable;      // Enable Multi-Viewport / Platform Windows
        io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines;
        io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
        io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;

        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.ConfigDragClickToInputText = true;
        io.ConfigDockingTransparentPayload = true;

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }


        //--ImGUI Configuration--------------------
        //std::ifstream ifile("imgui.ini");
        //ini_file_exists = ifile.is_open();
        //ifile.close();
        //
        //if (ini_file_exists)
        //    ImGui::LoadIniSettingsFromDisk("imgui.ini");

        //FONTS
        //io.Fonts->AddFontFromFileTTF("assets/default/Fonts/sourceSans/SourceSansPro-Regular.otf", 18);
        //io.Fonts->AddFontFromFileTTF("assets/default/Fonts/source-code-pro/SourceCodePro-Regular.otf", 26);

        // Despu�s de cargar las fuentes, llama a esta funci�n
        //ImGui_ImplOpenGL3_CreateFontsTexture();

        //--PANELS
        assetsPanel = CreateScope<AssetsPanel>();

        RegisterPopup("Save Scene As", [this]() {
            static char sceneNameBuffer[128] = "";
            ImGui::Text("Enter scene name:");
            ImGui::InputText("##SceneName", sceneNameBuffer, IM_ARRAYSIZE(sceneNameBuffer));

            if (ImGui::Button("Save", ImVec2(120, 0))) {
                EngineOpenGL::GetInstance().currentScene->SerializeScene(sceneNameBuffer);
                this->ClosePopup("Save Scene As");
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                this->ClosePopup("Save Scene As");
            }
            });
    }
    GuiLayer::~GuiLayer() {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    //-------------------------------------------------------------------------------
    

    //--LIFE-CYCLE
    void GuiLayer::begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }
    void GuiLayer::Draw()
    {
        begin();
        renderMainMenuBar();
        renderDockers();

        if (EngineOpenGL::GetInstance().engineState == EDITOR || EngineOpenGL::GetInstance().engineState == EDITOR_PLAY)
        {
            ShowPopups(); // Llamada para renderizar todos los popups



            //--SELECT MODEL FROM RAY POPUP
            if (isSelectingObject == true && !popupJustClosed)
            {
                ImGui::OpenPopup("Select Model");
                allowPopupClose = false; // Deshabilitar el cierre inmediato
                popupOpenTime = std::chrono::steady_clock::now(); // Marcar el tiempo de apertura
            }

            if (ImGui::BeginPopup("Select Model"))
            {
                // Habilitar el cierre después de que haya pasado suficiente tiempo
                auto currentTime = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - popupOpenTime).count() > 500)
                {
                    allowPopupClose = true;
                }

                // Verifica si se hace clic fuera del popup
                if (allowPopupClose && ImGui::IsMouseClicked(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
                {
                    libCore::EntityManager::GetInstance().currentSelectedEntityInScene = entt::null;
                    EntityManager::GetInstance().entitiesInRay.clear();
                    isSelectingObject = false; // Esta asignación cerrará el popup al finalizar el frame
                    showModelSelectionCombo = false;
                    ImGui::CloseCurrentPopup();
                    popupCloseTime = std::chrono::steady_clock::now();
                    popupJustClosed = true;
                }

                if (ImGui::Button("Cancel")) {
                    libCore::EntityManager::GetInstance().currentSelectedEntityInScene = entt::null;
                    EntityManager::GetInstance().entitiesInRay.clear();
                    isSelectingObject = false; // Esta asignación cerrará el popup al finalizar el frame
                    showModelSelectionCombo = false;
                    ImGui::CloseCurrentPopup();
                    popupCloseTime = std::chrono::steady_clock::now();
                    popupJustClosed = true;
                }
                for (const auto& entity : EntityManager::GetInstance().entitiesInRay) {
                    // Verificar si la entidad tiene un TagComponent
                    if (EntityManager::GetInstance().m_registry->has<TagComponent>(entity)) {
                        auto& tagComponent = EntityManager::GetInstance().m_registry->get<TagComponent>(entity);

                        // Mostrar el nombre del TagComponent en el botón
                        if (ImGui::Button(tagComponent.Tag.c_str())) {
                            EntityManager::GetInstance().currentSelectedEntityInScene = entity;
                            isSelectingObject = false; // Esta asignación cerrará el popup al finalizar el frame
                            ImGui::CloseCurrentPopup();
                            popupCloseTime = std::chrono::steady_clock::now();
                            popupJustClosed = true;
                        }
                    }
                }
                ImGui::EndPopup();
            }
            else if (popupJustClosed)
            {
                auto currentTime = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - popupCloseTime).count() > 500)
                {
                    popupJustClosed = false;
                }
            }
            //--------------------------------------------------------

            //--CHECK ImGizmo
            checkGizmo(ViewportManager::GetInstance().viewports[EngineOpenGL::GetInstance().currentViewport]);
            //--------------------------------------------------------

            DrawHierarchyPanel();
            DrawSelectedEntityComponentsPanel();
            DrawLightsPanel(LightsManager::GetLights());
            DrawMaterialsPanel();
            DrawTexturesPanel();
            DrawModelsPanel();
            DrawLogPanel();
            //RenderCheckerMatrix(); //Panel para el editor de roofs

            Renderer::getInstance().ShowControlsGUI();
            ViewportManager::GetInstance().DrawPanelGUI();
            //--------------------------------------------------------


            //-------------------------------------------ASSETS PANEL--------------------------------------
            assetsPanel->OnImGuiRender();
            //------------------------------------------------------------------------------------------------ 

            //-------------------------------------------TOOLBAR PANEL--------------------------------------
            DrawToolBarEditor();
            //------------------------------------------------------------------------------------------------ 
        }
        else if (EngineOpenGL::GetInstance().engineState == PLAY)
        {

        }

        end();
    }
    void GuiLayer::renderDockers()
    {
        // Configuración del Docking
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        float toolbarHeight = 40.0f;
        //// Configuración del Docking
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + ImGui::GetFrameHeight() + toolbarHeight));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - ImGui::GetFrameHeight() - toolbarHeight));
        /*ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);*/
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;//  ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // Make dockspace background transparent

        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        // DockSpace
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

        //ImGui::End();
        ImGui::PopStyleColor(); // Revert the transparent background color
    }
    //-------------------------------------------------------------------------------


    //--TOP MAIN MENU & TOOLBAR
    void GuiLayer::renderMainMenuBar() 
    {

        // Barra de menu principal
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Scene"))
            {
                if (ImGui::MenuItem("New Scene")) { /* Abrir archivo */ }

                // Lógica para abrir el popup de guardado
                if (ImGui::MenuItem("Save Scene"))
                {
                    OpenPopup("Save Scene As");
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("GameObjects"))
            {
                if (ImGui::MenuItem("Check Entities"))
                {
                    EntityManager::GetInstance().DebugPrintAllEntityHierarchies();
                }

                
                if (ImGui::MenuItem("Empty"))
                {
                    EntityManager::GetInstance().CreateEmptyGameObject("new_GameObject");
                }

                // Lights Section
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(1, 1, 1, 1), "Lights");
                ImGui::Separator();

                if (ImGui::MenuItem("Directional Light"))
                {
                    libCore::LightsManager::CreateDirectionalLight();
                }
                if (ImGui::MenuItem("Point Light"))
                {
                    libCore::LightsManager::CreateLight(true, libCore::LightType::POINT, glm::vec3(0.0f, 0.0f, 0.0f));
                }
                if (ImGui::MenuItem("Area Light"))
                {
                    libCore::LightsManager::CreateLight(true, libCore::LightType::AREA, glm::vec3(0.0f, 0.0f, 0.0f));
                }

                // Prefabs
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Spacing();
                if (ImGui::MenuItem("Camera"))
                {
                    EntityManager::GetInstance().CreateCamera();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
    void GuiLayer::DrawToolBarEditor()
    {
        // Barra de herramientas ubicada debajo del menú superior
        ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, 40));

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        if (ImGui::Begin("Toolbar", nullptr, window_flags))
        {
            if (ImGui::Button("Editor")) {
                EngineOpenGL::GetInstance().ChangeEngineState(EngineStates::EDITOR);
            }
            ImGui::SameLine();
            if (ImGui::Button("Editor Play")) {
                EngineOpenGL::GetInstance().ChangeEngineState(EngineStates::EDITOR_PLAY);
            }
            ImGui::SameLine();
            if (ImGui::Button("Play")) {
                EngineOpenGL::GetInstance().ChangeEngineState(EngineStates::PLAY);
            }

            //-- ImGIZMO CONTROLS AREA
            static bool useLocalTransform = true; // Variable para controlar el modo de transformación
            static bool snapEnabled = false; // Variable para controlar si el snap está habilitado
            static float snapValue[3] = { 1.0f, 1.0f, 1.0f }; // Valores de snap para mover, rotar, y escalar

            // ImGui button to toggle transform mode
            ImGui::SameLine();
            if (ImGui::Button(useLocalTransform ? "Switch to Global" : "Switch to Local"))
            {
                useLocalTransform = !useLocalTransform;
                m_useLocalTransform = useLocalTransform;
            }
            ImGui::SameLine();
            // Toggle snap with a button
            if (ImGui::Button(snapEnabled ? "Disable Snap" : "Enable Snap"))
            {
                snapEnabled = !snapEnabled;
                m_snapEnabled = snapEnabled;
            }
            //--
        }
        ImGui::End();

        /*if (ImGui::Begin("Engine State")) {
            if (ImGui::Button("Editor")) {
                EngineOpenGL::GetInstance().ChangeEngineState(EngineStates::EDITOR);
            }
            ImGui::SameLine();
            if (ImGui::Button("Editor Play")) {
                EngineOpenGL::GetInstance().ChangeEngineState(EngineStates::EDITOR_PLAY);
            }
            ImGui::SameLine();
            if (ImGui::Button("Play")) {
                EngineOpenGL::GetInstance().ChangeEngineState(EngineStates::PLAY);
            }
        }
        ImGui::End();*/
    }
    //-------------------------------------------------------------------------------


    //--POPUPS
    void GuiLayer::RegisterPopup(const std::string& title, const std::function<void()>& content)
    {
        popups.emplace_back(title, content);
    }
    void libCore::GuiLayer::ShowPopups()
    {
        for (auto& popup : popups) {
            if (popup.isOpen) {
                ImGui::OpenPopup(popup.title.c_str());
            }

            if (ImGui::BeginPopupModal(popup.title.c_str(), &popup.isOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
                popup.content();
                ImGui::EndPopup();
            }
        }
    }
    void libCore::GuiLayer::OpenPopup(const std::string& title)
    {
        for (auto& popup : popups) {
            if (popup.title == title) {
                popup.isOpen = true;
                break;
            }
        }
    }
    void libCore::GuiLayer::ClosePopup(const std::string& title)
    {
        for (auto& popup : popups) {
            if (popup.title == title) {
                popup.isOpen = false;
                break;
            }
        }
    }
    //-------------------------------------------------------------------------------


    //--UPDATES
    void GuiLayer::checkGizmo(const Ref<Viewport>& viewport)
    {

        ////--INPUTS TOOLS
        if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_T))
        {
            m_GizmoOperation = GizmoOperation::Translate;
        }
        else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_Y))
        {
            m_GizmoOperation = GizmoOperation::Rotate3D;
        }
        else if (InputManager::Instance().IsKeyJustPressed(GLFW_KEY_U))
        {
            m_GizmoOperation = GizmoOperation::Scale;
        }
        ////------------------------------------------------------------------------------

        //---------------------------ImGUIZMO------------------------------------------
        if (EntityManager::GetInstance().currentSelectedEntityInScene != entt::null)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(viewport->viewportPos.x,
                viewport->viewportPos.y,
                viewport->viewportSize.x,
                viewport->viewportSize.y);

            // Obtén las matrices correctas
            glm::mat4 camera_view = glm::lookAt(viewport->camera->Position, viewport->camera->Position + viewport->camera->Orientation, viewport->camera->Up);
            glm::mat4 camera_projection = viewport->camera->projection;

            auto& transformComponent = EntityManager::GetInstance().GetComponent<TransformComponent>(EntityManager::GetInstance().currentSelectedEntityInScene);

            // Usar la transformación global
            glm::mat4 entity_transform = transformComponent.getGlobalTransform(EntityManager::GetInstance().currentSelectedEntityInScene, *EntityManager::GetInstance().m_registry);

            ImGuizmo::MODE transformMode = m_useLocalTransform ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

            // Habilitar el snap si está activo
            bool snap = m_snapEnabled;
            float* snapValues = nullptr;

            switch (m_GizmoOperation)
            {
            case GizmoOperation::Translate:
                if (snap) snapValues = m_snapValue; // Snap en las tres dimensiones (X, Y, Z)
                ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
                    ImGuizmo::TRANSLATE, transformMode, glm::value_ptr(entity_transform), nullptr, snapValues);
                break;
            case GizmoOperation::Rotate3D:
                if (snap) m_snapValue[0] = 15.0f; // Snap en ángulos de 15 grados (esto depende de tus preferencias)
                ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
                    ImGuizmo::ROTATE, transformMode, glm::value_ptr(entity_transform), nullptr, snap ? m_snapValue : nullptr);
                break;
            case GizmoOperation::Scale:
                if (snap) snapValues = m_snapValue; // Snap en las tres dimensiones (X, Y, Z)
                ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
                    ImGuizmo::SCALE, transformMode, glm::value_ptr(entity_transform), nullptr, snapValues);
                break;
            }

            if (ImGuizmo::IsOver())
            {
                EngineOpenGL::GetInstance().usingGizmo = true;
            }
            else
            {
                EngineOpenGL::GetInstance().usingGizmo = false;
            }

            if (ImGuizmo::IsUsing())
            {
                // Actualizar la transformación local basada en la transformación global manipulada
                transformComponent.setTransformFromGlobal(entity_transform, EntityManager::GetInstance().currentSelectedEntityInScene, *EntityManager::GetInstance().m_registry);
            }
        }
    }
    //-------------------------------------------------------------------------------
 

    //--HIREARCHY PANEL
    void GuiLayer::DrawHierarchyPanel() {
        ImGui::Begin("Hierarchy");

        // Iterar sobre las entidades raíz (aquellas que no tienen un ParentComponent)
        auto view = EntityManager::GetInstance().m_registry->view<TransformComponent>(entt::exclude<ParentComponent>);

        for (auto entity : view) {
            DrawEntityNode(entity);
        }

        ImGui::End();
    }
    void GuiLayer::DrawEntityNode(entt::entity entity) {
        ImGui::PushID(static_cast<int>(entity));

        auto& tagComponent = EntityManager::GetInstance().m_registry->get<TagComponent>(entity);
        auto& idComponent = EntityManager::GetInstance().m_registry->get<IDComponent>(entity);

        // Obtener el UUID como cadena
        std::string uuidStr = std::to_string(static_cast<uint64_t>(idComponent.ID));

        // Verificar si la entidad es padre o hijo
        bool isParent = EntityManager::GetInstance().m_registry->has<ChildComponent>(entity);
        bool isChild = EntityManager::GetInstance().m_registry->has<ParentComponent>(entity);

        // Usar el nombre del TagComponent
        std::string nodeName = tagComponent.Tag;

        // Comprobar si la entidad está seleccionada
        bool isSelected = EntityManager::GetInstance().currentSelectedEntityInScene == entity;

        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (isSelected ? ImGuiTreeNodeFlags_Selected : 0);
        if (!isParent) {
            nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)entity, nodeFlags, "%s", nodeName.c_str());
        if (ImGui::IsItemClicked()) {
            EntityManager::GetInstance().currentSelectedEntityInScene = entity;
        }

        // Handle drag source
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("ENTITY_PAYLOAD", &entity, sizeof(entt::entity));
            ImGui::Text("Moving %s", nodeName.c_str());
            ImGui::EndDragDropSource();
        }

        // Handle drag target
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_PAYLOAD")) {
                IM_ASSERT(payload->DataSize == sizeof(entt::entity));
                entt::entity droppedEntity = *(const entt::entity*)payload->Data;
                EntityManager::GetInstance().AddChild(entity, droppedEntity);
            }
            ImGui::EndDragDropTarget();
        }

        if (nodeOpen && isParent) {
            auto& children = EntityManager::GetInstance().m_registry->get<ChildComponent>(entity).children;
            for (auto child : children) {
                DrawEntityNode(child);
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
    //-------------------------------------------------------------------------------
    

    //--INSPECTOR PANEL
    void GuiLayer::DrawSelectedEntityComponentsPanel()
    {
        ImGui::Begin("Inspector");

        entt::entity selectedEntity = EntityManager::GetInstance().currentSelectedEntityInScene;

        if (selectedEntity != entt::null && EntityManager::GetInstance().m_registry->valid(selectedEntity))
        {
            DrawComponentEditor(selectedEntity);

            //--UUID_COMPONENT
            if (EntityManager::GetInstance().HasComponent<IDComponent>(selectedEntity)) {
                auto& idComponent = EntityManager::GetInstance().GetComponent<IDComponent>(selectedEntity);
                if (ImGui::CollapsingHeader("ID")) {
                    std::string uuidStr = std::to_string(static_cast<uint64_t>(idComponent.ID));
                    ImGui::Text("UUID: %s", uuidStr.c_str());

                    // Botón para eliminar la entidad
                    if (ImGui::Button("Delete Entity")) {
                        EntityManager::GetInstance().MarkToDeleteRecursively(EntityManager::GetInstance().currentSelectedEntityInScene);
                        EntityManager::GetInstance().currentSelectedEntityInScene = entt::null;
                    }
                }
            }
            //--TAG_COMPONENT
            if (EntityManager::GetInstance().HasComponent<TagComponent>(selectedEntity)) {
                auto& tagComponent = EntityManager::GetInstance().GetComponent<TagComponent>(selectedEntity);
                if (ImGui::CollapsingHeader("Tag")) {
                    // Mostrar el tag actual
                    ImGui::Text("Current Tag: %s", tagComponent.Tag.c_str());

                    // Definir un buffer estático para el nuevo nombre del tag
                    static char buffer[256];

                    // Inicializar el buffer con el valor actual del tagComponent solo si el buffer está vacío
                    if (strlen(buffer) == 0) {
                        strncpy_s(buffer, tagComponent.Tag.c_str(), sizeof(buffer));
                        buffer[sizeof(buffer) - 1] = '\0'; // Asegurarse de que el buffer está null-terminated
                    }

                    // Mostrar el campo de texto editable y el botón para actualizar el tag
                    if (ImGui::InputText("New Tag", buffer, sizeof(buffer))) {
                        // El buffer se actualiza automáticamente al escribir
                    }

                    if (ImGui::Button("Update Tag")) {
                        // Actualizar el tagComponent con el nuevo valor del buffer
                        tagComponent.Tag = std::string(buffer);

                        // Limpiar el buffer después de actualizar
                        buffer[0] = '\0';
                    }
                }
            }
            //--PARENT_COMPONENT
            if (EntityManager::GetInstance().HasComponent<ParentComponent>(selectedEntity)) {
                auto& parentComponent = EntityManager::GetInstance().GetComponent<ParentComponent>(selectedEntity);
                if (ImGui::CollapsingHeader("Parent")) {
                    std::string parentTag = "None";
                    if (EntityManager::GetInstance().HasComponent<TagComponent>(parentComponent.parent)) {
                        parentTag = EntityManager::GetInstance().GetComponent<TagComponent>(parentComponent.parent).Tag;
                    }
                    ImGui::Text("Parent Entity: %s", parentTag.c_str());
                }
            }
            //--CHILD_COMPONENT
            if (EntityManager::GetInstance().HasComponent<ChildComponent>(selectedEntity)) {
                auto& childComponent = EntityManager::GetInstance().GetComponent<ChildComponent>(selectedEntity);
                if (ImGui::CollapsingHeader("Children")) {
                    for (auto child : childComponent.children) {
                        if (EntityManager::GetInstance().HasComponent<TagComponent>(child)) {
                            std::string childTag = EntityManager::GetInstance().GetComponent<TagComponent>(child).Tag;
                            ImGui::Text("Child Entity: %s", childTag.c_str());
                        }
                    }
                }
            }
            //--TRANSFORM_COMPONENT
            if (EntityManager::GetInstance().HasComponent<TransformComponent>(selectedEntity)) {
                auto& transformComponent = EntityManager::GetInstance().GetComponent<TransformComponent>(selectedEntity);
                if (ImGui::CollapsingHeader("Transform")) {
                    auto& transform = transformComponent.transform;

                    // Posición
                    ImGui::Text("Position");
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
                    ImGui::SameLine();
                    ImGui::DragFloat("##PosX", &transform->position.x, 0.1f, -FLT_MAX, FLT_MAX, "X: %.2f");
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
                    ImGui::SameLine();
                    ImGui::DragFloat("##PosY", &transform->position.y, 0.1f, -FLT_MAX, FLT_MAX, "Y: %.2f");
                    ImGui::TextColored(ImVec4(0, 0, 1, 1), "Z");
                    ImGui::SameLine();
                    ImGui::DragFloat("##PosZ", &transform->position.z, 0.1f, -FLT_MAX, FLT_MAX, "Z: %.2f");

                    // Rotación
                    ImGui::Text("Rotation");
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
                    ImGui::SameLine();
                    if (ImGui::DragFloat("##RotX", &transform->eulerAngles.x, 0.1f, -360.0f, 360.0f, "X: %.2f")) {
                        transform->updateRotationFromEulerAngles();  // Actualizar la rotación cuando se cambien los ángulos de Euler
                    }
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
                    ImGui::SameLine();
                    if (ImGui::DragFloat("##RotY", &transform->eulerAngles.y, 0.1f, -360.0f, 360.0f, "Y: %.2f")) {
                        transform->updateRotationFromEulerAngles();  // Actualizar la rotación cuando se cambien los ángulos de Euler
                    }
                    ImGui::TextColored(ImVec4(0, 0, 1, 1), "Z");
                    ImGui::SameLine();
                    if (ImGui::DragFloat("##RotZ", &transform->eulerAngles.z, 0.1f, -360.0f, 360.0f, "Z: %.2f")) {
                        transform->updateRotationFromEulerAngles();  // Actualizar la rotación cuando se cambien los ángulos de Euler
                    }

                    // Escala
                    ImGui::Text("Scale");
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
                    ImGui::SameLine();
                    ImGui::DragFloat("##ScaleX", &transform->scale.x, 0.01f, 0.0f, FLT_MAX, "X: %.2f");
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
                    ImGui::SameLine();
                    ImGui::DragFloat("##ScaleY", &transform->scale.y, 0.01f, 0.0f, FLT_MAX, "Y: %.2f");
                    ImGui::TextColored(ImVec4(0, 0, 1, 1), "Z");
                    ImGui::SameLine();
                    ImGui::DragFloat("##ScaleZ", &transform->scale.z, 0.01f, 0.0f, FLT_MAX, "Z: %.2f");
                }
            }

            //--MESH_COMPONENT
            if (EntityManager::GetInstance().HasComponent<MeshComponent>(selectedEntity)) {
                if (ImGui::CollapsingHeader("Mesh")) {
                    auto& meshComponent = EntityManager::GetInstance().GetComponent<MeshComponent>(selectedEntity);
                    ImGui::Text("Mesh Name: %s", meshComponent.mesh->meshName.c_str());
                    ImGui::Text("Instance: %s", meshComponent.isInstance ? "Yes" : "No");
                    if (meshComponent.isInstance) {
                        ImGui::Text("Original Model: %s", meshComponent.originalModel->name.c_str());
                    }
                }
            }
            //--AABB_COMPONENT
            if (EntityManager::GetInstance().HasComponent<AABBComponent>(selectedEntity)) {
                if (ImGui::CollapsingHeader("AABB")) {
                    auto& aabbComponent = EntityManager::GetInstance().GetComponent<AABBComponent>(selectedEntity);
                    ImGui::Checkbox("Show ABB", &aabbComponent.aabb->showAABB);
                }
            }
            //--MATERIAL_COMPONENT
            if (EntityManager::GetInstance().HasComponent<MaterialComponent>(selectedEntity)) {
                auto& materialComponent = EntityManager::GetInstance().GetComponent<MaterialComponent>(selectedEntity);
                auto& material = *materialComponent.material;

                if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Text("Material Name: %s", material.materialName.c_str());
                    // Drag Target for the whole material
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIAL")) {
                            IM_ASSERT(payload->DataSize == sizeof(Ref<Material>));
                            Ref<Material> newMaterial = *(const Ref<Material>*)payload->Data;
                            materialComponent.material = newMaterial;
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::Text("Shader Name: %s", material.shaderName.c_str());

                    ImGui::ColorEdit3("Albedo Color", (float*)&materialComponent.material->albedoColor);
                    ImGui::DragFloat("Normal Strength", &materialComponent.material->normalStrength, 0.1f, -10.0f, 10.0f);
                    ImGui::DragFloat("Metallic Value", &materialComponent.material->metallicValue, 0.1f, 0.0f, 10.0f);
                    ImGui::DragFloat("Roughness Value", &materialComponent.material->roughnessValue, 0.1f, 0.0f, 10.0f);

                    auto drawTextureSlot = [](const char* label, Ref<Texture>& texture) {
                        ImGui::Text("%s", label);
                        if (texture && texture->IsValid()) {
                            ImGui::Image((void*)(intptr_t)texture->GetTextureID(), ImVec2(128, 128));
                        }
                        else {
                            ImGui::Text("None");
                        }

                        if (ImGui::BeginDragDropTarget()) {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE")) {
                                IM_ASSERT(payload->DataSize == sizeof(Ref<Texture>));
                                texture = *(const Ref<Texture>*)payload->Data;
                            }
                            ImGui::EndDragDropTarget();
                        }
                    };

                    drawTextureSlot("Albedo Map", materialComponent.material->albedoMap);
                    drawTextureSlot("Normal Map", materialComponent.material->normalMap);
                    drawTextureSlot("Metallic Map", materialComponent.material->metallicMap);
                    drawTextureSlot("Roughness Map", materialComponent.material->roughnessMap);

                    
                }
            }
            //--CAMERA_COMPONENT
            if (EntityManager::GetInstance().HasComponent<CameraComponent>(selectedEntity)) {
                auto& cameraComponent = EntityManager::GetInstance().GetComponent<CameraComponent>(selectedEntity);
                if (ImGui::CollapsingHeader("Camera")) 
                {
                    ImGui::Text("Width: %d", cameraComponent.camera->width);
                    ImGui::Text("Height: %d", cameraComponent.camera->height);

                    ImGui::Text("Up Vector");
                    ImGui::DragFloat3("Up", glm::value_ptr(cameraComponent.camera->Up), 0.1f);

                    ImGui::Text("FOV: %.2f", cameraComponent.camera->FOVdeg);
                    ImGui::DragFloat("FOV", &cameraComponent.camera->FOVdeg, 0.1f, 1.0f, 180.0f);

                    ImGui::Text("Near Plane: %.2f", cameraComponent.camera->nearPlane);
                    ImGui::DragFloat("Near Plane", &cameraComponent.camera->nearPlane, 0.01f, 0.01f, cameraComponent.camera->farPlane - 0.1f);

                    ImGui::Text("Far Plane: %.2f", cameraComponent.camera->farPlane);
                    ImGui::DragFloat("Far Plane", &cameraComponent.camera->farPlane, 1.0f, cameraComponent.camera->nearPlane + 0.1f, 10000.0f);

                    ImGui::Text("View Matrix");
                    ImGui::InputFloat4("##ViewRow1", glm::value_ptr(cameraComponent.camera->view[0]));
                    ImGui::InputFloat4("##ViewRow2", glm::value_ptr(cameraComponent.camera->view[1]));
                    ImGui::InputFloat4("##ViewRow3", glm::value_ptr(cameraComponent.camera->view[2]));
                    ImGui::InputFloat4("##ViewRow4", glm::value_ptr(cameraComponent.camera->view[3]));

                    ImGui::Text("Projection Matrix");
                    ImGui::InputFloat4("##ProjRow1", glm::value_ptr(cameraComponent.camera->projection[0]));
                    ImGui::InputFloat4("##ProjRow2", glm::value_ptr(cameraComponent.camera->projection[1]));
                    ImGui::InputFloat4("##ProjRow3", glm::value_ptr(cameraComponent.camera->projection[2]));
                    ImGui::InputFloat4("##ProjRow4", glm::value_ptr(cameraComponent.camera->projection[3]));

                    ImGui::Text("Camera Matrix");
                    ImGui::InputFloat4("##CamRow1", glm::value_ptr(cameraComponent.camera->cameraMatrix[0]));
                    ImGui::InputFloat4("##CamRow2", glm::value_ptr(cameraComponent.camera->cameraMatrix[1]));
                    ImGui::InputFloat4("##CamRow3", glm::value_ptr(cameraComponent.camera->cameraMatrix[2]));
                    ImGui::InputFloat4("##CamRow4", glm::value_ptr(cameraComponent.camera->cameraMatrix[3]));
                }
            }
            //--SCRIPT_COMPONENT
            if (EntityManager::GetInstance().HasComponent<ScriptComponent>(selectedEntity)) {
                auto& scriptComponent = EntityManager::GetInstance().GetComponent<ScriptComponent>(selectedEntity);
                if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Text("Script Instance: %s", scriptComponent.instance ? typeid(*scriptComponent.instance).name() : "None");
                    // Aquí puedes agregar más controles para interactuar con el script, si es necesario

                    // Si instance es nullptr, muestra la opción para asignar un script
                    if (!scriptComponent.instance) {
                        const auto& creators = ScriptFactory::GetInstance().GetCreators();
                        static std::string selectedScript;
                        std::vector<std::string> scriptNames;
                        for (const auto& pair : creators) {
                            scriptNames.push_back(pair.first);
                        }

                        if (ImGui::BeginCombo("Assign Script", selectedScript.c_str())) {
                            for (const auto& scriptName : scriptNames) {
                                bool isSelected = (selectedScript == scriptName);
                                if (ImGui::Selectable(scriptName.c_str(), isSelected)) {
                                    selectedScript = scriptName;
                                }
                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }

                        if (ImGui::Button("Assign") && !selectedScript.empty()) {
                            scriptComponent.instance = ScriptFactory::GetInstance().CreateScript(selectedScript);
                            scriptComponent.instance->SetEntity(selectedEntity, EntityManager::GetInstance().m_registry);
                            //scriptComponent.OnAssign(); // Inicializar el script
                            selectedScript.clear(); // Limpiar la selección después de asignar el script
                        }
                    }
                }
            }
        }
        else {
            ImGui::Text("No entity selected.");
        }

        ImGui::End();
    }
    //-------------------------------------------------------------------------------

    //--MATERIAL PANEL
    void GuiLayer::DrawMaterialsPanel() {
        ImGui::Begin("Materials In Scene");

        // Añadir un control deslizante para ajustar el tamaño de la previsualización
        float previewSize = 64.0f; // Tamaño inicial de la previsualización
        float cellPadding = 0.0f;  // Padding inicial entre celdas
        ImGui::SliderFloat("Preview Size", &previewSize, 32.0f, 256.0f);

        for (auto& pair : AssetsManager::GetInstance().GetAllMaterials()) {
            std::string key = pair.first;
            Ref<Material> material = pair.second;

            if (key.empty()) {
                key = "Unnamed Material";
            }

            std::string nodeId = key + "##" + key;

            if (ImGui::TreeNode(nodeId.c_str(), "Material: %s", material->materialName.c_str())) {

                ImGui::Spacing();

                //-Drag Source
                ImGui::Text("Material: %s", material->materialName.c_str());
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    ImGui::SetDragDropPayload("MATERIAL", &material, sizeof(Ref<Material>)); // Set payload
                    ImGui::Text("Drag Material: %s", material->materialName.c_str());
                    ImGui::EndDragDropSource();
                }
                //----------------------

                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::ColorEdit3("Albedo Color", (float*)&material->albedoColor);
                ImGui::DragFloat("Normal Strength", &material->normalStrength, 0.1f, -10.0f, 10.0f);
                ImGui::DragFloat("Metallic Value", &material->metallicValue, 0.1f, 0.0f, 10.0f);
                ImGui::DragFloat("Roughness Value", &material->roughnessValue, 0.1f, 0.0f, 10.0f);

                // Calcular el número de columnas basado en el tamaño de la imagen y el padding
                float panelWidth = ImGui::GetContentRegionAvail().x;
                int columns = static_cast<int>(panelWidth / (previewSize + cellPadding));
                if (columns < 1) columns = 1;

                ImGui::Columns(columns, nullptr, false);

                // Display material textures and allow them to accept drag and drop
                auto drawTextureSlot = [previewSize, cellPadding](const char* label, Ref<Texture>& texture) {
                    ImGui::BeginGroup(); // Comienza un grupo para que el nombre y la imagen estén juntos
                    ImGui::Text("%s", label);
                    if (texture && texture->IsValid()) {
                        ImGui::Image((void*)(intptr_t)texture->GetTextureID(), ImVec2(previewSize, previewSize));
                    }
                    else {
                        ImGui::Text("None");
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE")) {
                            IM_ASSERT(payload->DataSize == sizeof(Ref<Texture>));
                            texture = *(const Ref<Texture>*)payload->Data;
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::EndGroup(); // Termina el grupo
                    ImGui::SameLine(); // Coloca el siguiente elemento en la misma línea
                    ImGui::Dummy(ImVec2(0.0f, cellPadding)); // Añadir espacio extra entre celdas
                    ImGui::NextColumn();
                };

                drawTextureSlot("Albedo Map", material->albedoMap);
                drawTextureSlot("Normal Map", material->normalMap);
                drawTextureSlot("Metallic Map", material->metallicMap);
                drawTextureSlot("Roughness Map", material->roughnessMap);

                ImGui::Columns(1); // Volver a una sola columna
                ImGui::NewLine(); // Asegura que el siguiente elemento se coloque en una nueva línea

                ImGui::TreePop();
            }

            ImGui::Separator();
        }

        ImGui::End();
    }
    //-----------------------------------------------------------------------------------------------------

    //--GLOBAL TEXTURES PANEL
    void GuiLayer::DrawTexturesPanel() {
        auto& assetsManager = libCore::AssetsManager::GetInstance();
        const auto& textures = assetsManager.GetAllTextures();
        std::size_t numberOfTextures = assetsManager.GetNumberOfTextures();

        static float imageSize = 128.0f; // Tamaño inicial de la imagen
        static float cellPadding = 0.0f; // Padding inicial entre celdas

        ImGui::Begin("Textures Panel");

        ImGui::Spacing();
        ImGui::Text("Texture Count: %zu", numberOfTextures);
        ImGui::Spacing();

        ImGui::SliderFloat("Image Size", &imageSize, 32.0f, 256.0f);
        //ImGui::SliderFloat("Cell Padding", &cellPadding, 0.0f, 20.0f);
        ImGui::Spacing();

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columns = static_cast<int>(panelWidth / (imageSize + cellPadding));
        if (columns < 1) columns = 1;

        ImGui::Columns(columns, nullptr, false);

        for (const auto& texturePair : textures) {
            const auto& textureName = texturePair.first;
            const auto& texture = texturePair.second;

            if (texture != nullptr && texture->IsValid()) {
                ImGui::PushID(textureName.c_str());

                //-Drag area
                ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture->GetTextureID())), ImVec2(imageSize, imageSize));
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    ImGui::SetDragDropPayload("TEXTURE", &texture, sizeof(Ref<Texture>));
                    ImGui::Text("Drag Texture: %s", textureName.c_str());
                    ImGui::EndDragDropSource();
                }
                //-----------

                ImGui::TextWrapped("Name: %s", textureName.c_str());
                //ImGui::TextWrapped("Path: %s", texture->texturePath.c_str());
                ImGui::TextWrapped("Slot: %u", texture->m_unit);

                ImGui::Dummy(ImVec2(0.0f, cellPadding));
                ImGui::NextColumn();

                ImGui::PopID();
            }
            else {
                ImGui::Text("Invalid texture: %s", textureName.c_str());
            }
        }

        ImGui::Columns(1);
        ImGui::End();
    }
    //-----------------------------------------------------------------------------------------------------

    //--GLOBAL MODELS PANEL  
    // Renderiza el árbol de jerarquía de modelos y sus meshes
    void GuiLayer::DrawModelHierarchy(const Ref<libCore::Model>& model)
    {
        if (!model) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);

        // Crear un nodo del árbol para el modelo
        if (ImGui::TreeNode(model->name.c_str()))
        {
            // Recorrer los meshes del modelo
            for (const auto& mesh : model->meshes)
            {
                DrawMeshWithThumbnail(mesh, model->modelParent->importModelData.filePath);
                
                if (ImGui::Button(("Instantiate " + model->name).c_str())) 
                {
                    // Lógica de instanciación del modelo
                    EntityManager::GetInstance().CreateGameObjectFromModel(model, entt::null);
                }
            }

            // Recorrer recursivamente los hijos del modelo
            for (const auto& child : model->children)
            {
                DrawModelHierarchy(child);
            }

            ImGui::TreePop();
        }
    }
    // Muestra el thumbnail junto con el nombre del mesh
    void GuiLayer::DrawMeshWithThumbnail(const Ref<Mesh>& mesh, const std::string& modelPath)
    {
        if (mesh->thumbnailTextureID == 0)
        {
            // Obtener la ruta absoluta del directorio de trabajo actual
            std::filesystem::path projectBasePath = std::filesystem::current_path();

            std::string assetsPath = "assets/models";

            // Construir la ruta completa para el thumbnail
            std::filesystem::path thumbnailPath = projectBasePath / modelPath / "thumbnails" / (mesh->meshName + "_thumbnail.png");

            // Convertir la ruta a string y cargar la textura
            mesh->thumbnailTextureID = TextureManager::getInstance().LoadTextureFromFile(thumbnailPath.string().c_str());
        }


        ImVec2 imageSize(128, 128);

        if (mesh->thumbnailTextureID != 0)
        {
            ImGui::Image((void*)(intptr_t)mesh->thumbnailTextureID, imageSize);
        }
        else
        {
            ImGui::Text("No thumbnail");
        }

        ImGui::SameLine();
        ImGui::Text("Mesh: %s", mesh->meshName.c_str());
    }
    // Panel de Modelos con la jerarquía y los thumbnails
    void GuiLayer::DrawModelsPanel()
    {
        auto& assetsManager = libCore::AssetsManager::GetInstance();
        const auto& models = assetsManager.GetAllModels();

        // Comenzar la ventana de ImGui
        ImGui::Begin("Models Panel");

        // Mostrar el número de modelos cargados
        ImGui::Spacing();
        ImGui::Text("Model Count: %zu", models.size());
        ImGui::Spacing();

        for (const auto& modelPair : models)
        {
            const auto& model = modelPair.second;

            if (model)
            {
                ImGui::Separator();
                ImGui::Text("Model: %s", model->name.c_str());

                if (ImGui::Button(("Instantiate " + model->name).c_str()))
                {
                    // Lógica de instanciación del modelo
                    EntityManager::GetInstance().CreateGameObjectFromModel(model, entt::null);
                }

                // Dibujar la jerarquía del modelo
                DrawModelHierarchy(model);
            }
            else
            {
                ImGui::Text("Invalid model");
            }
        }

        ImGui::End();
    }
    //-----------------------------------------------------------------------------------------------------


    //--GLOBAL LIGHT´s PANEL (ESTO CAMBIARÁ)
    void GuiLayer::DrawLightsPanel(const std::vector<Ref<libCore::Light>>& lightsInScene)
    {
        ImGui::Begin("Lights");

        for (size_t i = 0; i < lightsInScene.size(); ++i) {

            auto& light = lightsInScene[i];

            // Push a unique identifier onto the ID stack
            ImGui::PushID(static_cast<int>(i));

            std::string lightTypeStr;
            switch (light->type) {
            case LightType::POINT:       lightTypeStr = "Point"; break;
            case LightType::SPOT:        lightTypeStr = "Spot"; break;
            case LightType::AREA:        lightTypeStr = "Area"; break;
            case LightType::DIRECTIONAL: lightTypeStr = "Directional"; break;
            }

            std::string headerLabel = lightTypeStr + " Light: " + std::to_string(light->id);

            if (ImGui::CollapsingHeader(headerLabel.c_str())) {
                ImGui::Separator();

                // Common Controls
                ImGui::BulletText("Transform:");
                if (ImGui::DragFloat3("Position", &light->transform.position[0], 0.1f)) {
                    light->UpdateVertices(); // Actualiza los vértices cuando se cambie la posición
                }
                ImGui::DragFloat3("Rotation", &light->transform.rotation[0], 0.01f, -3.14159f, 3.14159f, "%.3f rad");
                ImGui::DragFloat3("Scale", &light->transform.scale[0], 0.1f, 0.01f, 100.0f, "%.3f");
                ImGui::Separator();

                // Attenuation parameters
                ImGui::DragFloat("Intensity", &light->intensity, 0.1f, 0.0f, 1000.0f, "%.2f");

                // Color Picker
                ImGui::ColorEdit3("Color", &light->color[0]);

                // Draw Debug
                ImGui::Checkbox("Debug", &light->showDebug);

                // Light Type
                static const char* lightTypes[] = { "Point", "Spot", "Area", "Directional" };
                ImGui::Combo("Type", reinterpret_cast<int*>(&light->type), lightTypes, IM_ARRAYSIZE(lightTypes));

                ImGui::Separator();

                // Specific Controls based on Light Type
                if (light->type == LightType::POINT)
                {
                    ImGui::DragFloat("Linear", &light->Linear, 0.01f, 0.0f, 10.0f, "%.2f");
                    ImGui::DragFloat("Quadratic", &light->Quadratic, 0.01f, 0.0f, 10.0f, "%.2f");
                    ImGui::DragFloat("Radius", &light->Radius, 0.01f, 0.0f, 100.0f, "%.2f");
                    ImGui::DragFloat("Border Smoothness", &light->LightSmoothness, 0.01f, 0.0f, 100.0f, "%.2f");
                }
                else if (light->type == LightType::SPOT)
                {
                    ImGui::BulletText("Spot Light Specifics:");
                    ImGui::DragFloat("Inner Cutoff", &light->innerCutoff, 0.01f, 0.0f, 1.0f, "%.2f");
                    ImGui::DragFloat("Outer Cutoff", &light->outerCutoff, 0.01f, 0.0f, 1.0f, "%.2f");
                    ImGui::DragFloat3("Direction", glm::value_ptr(light->transform.rotation), 0.01f, -1.0f, 1.0f, "%.2f");
                }
                else if (light->type == LightType::AREA)
                {
                    ImGui::BulletText("Area Light Specifics:");
                    ImGui::Checkbox("Two Sided", &light->twoSided);

                    for (size_t j = 0; j < light->areaLightpoints.size(); ++j) {
                        std::string pointLabel = "Point " + std::to_string(j);
                        if (ImGui::DragFloat3(pointLabel.c_str(), glm::value_ptr(light->areaLightpoints[j]), 0.1f)) {
                            light->UpdateVertices(); // Actualiza los vértices cuando se cambien los puntos del área
                        }
                    }

                    ImGui::Separator();

                    // LTC Parameters
                    ImGui::BulletText("LTC Parameters:");
                    if (ImGui::DragFloat("LUT_SIZE", &light->LUT_SIZE, 0.1f, 1.0f, 128.0f, "%.2f")) {
                        light->LUT_SCALE = (light->LUT_SIZE - 1.0f) / light->LUT_SIZE;
                        light->LUT_BIAS = 0.5f / light->LUT_SIZE;
                    }
                    ImGui::DragFloat("LUT_SCALE", &light->LUT_SCALE, 0.01f, 0.0f, 1.0f, "%.4f", ImGuiSliderFlags_ReadOnly);
                    ImGui::DragFloat("LUT_BIAS", &light->LUT_BIAS, 0.01f, 0.0f, 1.0f, "%.4f", ImGuiSliderFlags_ReadOnly);
                }
                else if (light->type == LightType::DIRECTIONAL)
                {
                    ImGui::Text("Directional Light");
                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                    ImGui::Checkbox("Debug", &light->showDebug);
                    ImGui::Separator();
                    ImGui::DragFloat("Intensity", &light->intensity, 0.1f, 0.0f, 1000.0f, "%.2f");
                    ImGui::Separator();

                    // Common Controls
                    ImGui::BulletText("Transform:");
                    if (ImGui::DragFloat3("Position", &light->transform.position[0], 0.1f)) {
                        light->UpdateVertices(); // Actualiza los vértices cuando se cambie la posición
                    }
                    ImGui::DragFloat3("Rotation", &light->transform.rotation[0], 0.01f, -3.14159f, 3.14159f, "%.3f rad");
                    ImGui::DragFloat3("Scale", &light->transform.scale[0], 0.1f, 0.01f, 100.0f, "%.3f");
                    ImGui::Separator();

                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                    ImGui::SliderFloat("Orbit X", &light->angleX, 0.0f, 6.28319f);
                    ImGui::SliderFloat("Orbit Y", &light->angleY, 0.0f, 6.28319f);
                    ImGui::SliderFloat("sceneRadiusOffset", &light->sceneRadiusOffset, 0.0f, 100.0f, "%.1f");
                    ImGui::SliderFloat("currentSceneRadius", &light->currentSceneRadius, 0.0f, 100.0f, "%.1f");

                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                    ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient));
                    ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse));
                    ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular));

                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                    ImGui::Checkbox("Draw Shadows", &light->drawShadows);

                    if (light->drawShadows) {
                        ImGui::SliderFloat("Shadow Intensity", &light->shadowIntensity, 0.0f, 1.0f);
                        ImGui::Checkbox("Use Poison Disk", &light->usePoisonDisk);

                        //ImGui::Dummy(ImVec2(0.0f, 20.0f));
                        //ImGui::Image((void*)(intptr_t)light->shadowTex, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));

                        ImGui::Dummy(ImVec2(0.0f, 20.0f));
                        ImGui::SliderFloat("Left", &light->orthoLeft, -100.0f, 100.0f);
                        ImGui::SliderFloat("Right", &light->orthoRight, -100.0f, 100.0f);
                        ImGui::SliderFloat("Top", &light->orthoTop, -100.0f, 100.0f);
                        ImGui::SliderFloat("Bottom", &light->orthoBottom, -100.0f, 100.0f);

                        ImGui::SliderFloat("Near", &light->orthoNear, 0.0f, 100.0f);
                        ImGui::SliderFloat("Near Offset", &light->orthoNearOffset, -100.0f, 100.0f);
                        ImGui::SliderFloat("Far", &light->orthoFar, 0.0f, 500.0f);
                        ImGui::SliderFloat("Far Offset", &light->orthoFarOffset, -100.0f, 100.0f);

                        ImGui::Dummy(ImVec2(0.0f, 20.0f));
                        if (ImGui::CollapsingHeader("Shadow Bias")) {
                            ImGui::InputFloat("00", &light->shadowBias[0][0], 0.001f);
                            ImGui::InputFloat("01", &light->shadowBias[0][1], 0.001f);
                            ImGui::InputFloat("02", &light->shadowBias[0][2], 0.001f);
                            ImGui::InputFloat("03", &light->shadowBias[0][3], 0.001f);

                            ImGui::InputFloat("10", &light->shadowBias[1][0], 0.001f);
                            ImGui::InputFloat("11", &light->shadowBias[1][1], 0.001f);
                            ImGui::InputFloat("12", &light->shadowBias[1][2], 0.001f);
                            ImGui::InputFloat("13", &light->shadowBias[1][3], 0.001f);

                            ImGui::InputFloat("20", &light->shadowBias[2][0], 0.001f);
                            ImGui::InputFloat("21", &light->shadowBias[2][1], 0.001f);
                            ImGui::InputFloat("22", &light->shadowBias[2][2], 0.001f);
                            ImGui::InputFloat("23", &light->shadowBias[2][3], 0.001f);

                            ImGui::InputFloat("30", &light->shadowBias[3][0], 0.001f);
                            ImGui::InputFloat("31", &light->shadowBias[3][1], 0.001f);
                            ImGui::InputFloat("32", &light->shadowBias[3][2], 0.001f);
                            ImGui::InputFloat("33", &light->shadowBias[3][3], 0.001f);

                            if (ImGui::Button("Reset")) {
                                light->shadowBias = glm::mat4(
                                    0.5, 0.0, 0.0, 0.0,
                                    0.0, 0.5, 0.0, 0.0,
                                    0.0, 0.0, 0.5, 0.0,
                                    0.5, 0.5, 0.5, 1.0
                                );
                            }
                        }
                    }

                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                }

                ImGui::Separator();
            }

            // Pop the identifier off the ID stack
            ImGui::PopID();
        }

        ImGui::End();
    }
    //-----------------------------------------------------------------------------------------------------


    //--SCRIPTS-EDITOR PANEL
    void GuiLayer::DrawComponentEditor(entt::entity entity) {
        auto& entityManager = EntityManager::GetInstance();

        static std::string selectedComponent;

        // Lista de todos los componentes disponibles, incluyendo ScriptComponent
        std::vector<std::string> componentNames = {
            "TransformComponent", "MeshComponent", "MaterialComponent",
            "CameraComponent","LightComponent", "DirectionalLightComponent", "ScriptComponent"
        };

        // ComboBox para seleccionar el componente
        if (ImGui::BeginCombo("Components", selectedComponent.c_str())) {
            for (const auto& componentName : componentNames) {
                bool isSelected = (selectedComponent == componentName);
                if (ImGui::Selectable(componentName.c_str(), isSelected)) {
                    selectedComponent = componentName;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // Botón para agregar el componente a la entidad
        if (ImGui::Button("Add Component") && !selectedComponent.empty()) {
            if (selectedComponent == "TransformComponent") {
                entityManager.AddComponent<TransformComponent>(entity);
            }
            else if (selectedComponent == "MeshComponent") {
                entityManager.AddComponent<MeshComponent>(entity);
            }
            else if (selectedComponent == "MaterialComponent") {
                entityManager.AddComponent<MaterialComponent>(entity);
            }
            else if (selectedComponent == "CameraComponent") {
                entityManager.AddComponent<CameraComponent>(entity);
            }
            /*else if (selectedComponent == "LightComponent") {
                entityManager.AddComponent<LightComponent>(entity);
            }
            else if (selectedComponent == "DirectionalLightComponent") {
                entityManager.AddComponent<DirectionalLightComponent>(entity);
            }*/
            else if (selectedComponent == "ScriptComponent") {
                entityManager.AddComponent<ScriptComponent>(entity);
            }
            selectedComponent.clear(); // Limpiar la selección después de agregar el componente
        }
    }
    //-----------------------------------------------------------------------------------------------------

    //--CONSOLE LOG PANEL
    void GuiLayer::DrawLogPanel() {
        static bool showInfo = true;
        static bool showWarning = true;
        static bool showError = true;

        ImGui::Begin("Console Log");

        if (ImGui::Button("Clear")) {
            ConsoleLog::GetInstance().ClearLogs();
        }

        ImGui::SameLine();
        ImGui::Checkbox("Info", &showInfo);
        ImGui::SameLine();
        ImGui::Checkbox("Warning", &showWarning);
        ImGui::SameLine();
        ImGui::Checkbox("Error", &showError);

        ImGui::Separator();

        ImGui::BeginChild("LogScrolling");
        const auto& logs = ConsoleLog::GetInstance().GetLogs();
        for (const auto& log : logs) {
            bool show = false;
            ImVec4 color;

            switch (log.level) {
            case LogLevel::L_INFO:
                show = showInfo;
                color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                break;
            case LogLevel::L_WARNING:
                show = showWarning;
                color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
                break;
            case LogLevel::L_ERROR:
                show = showError;
                color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                break;
            }

            if (show) {
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(log.message.c_str());
                ImGui::PopStyleColor();
            }
        }
        ImGui::EndChild();

        ImGui::End();
    }
    //-----------------------------------------------------------------------------------------------------
 

    //Especial para el editor de Roofs
    void GuiLayer::RenderCheckerMatrix() {
        const int xAxis = 10;
        const int yAxis = 10;
        const float scale = 1.0;

        static bool checkerMatrix[xAxis][yAxis] = { false }; // Matriz de checkboxes de 10x10
        static bool holeMatrix[xAxis][yAxis] = { false };    // Matriz de checkboxes para los agujeros
        static std::vector<Point> selectedPoints;     // Vector para almacenar los puntos seleccionados
        static std::vector<Point> holePoints;         // Vector para almacenar los puntos de los agujeros
        static std::vector<std::pair<int, int>> selectedOrder; // Para almacenar el orden de selección de celdas
        static std::vector<std::pair<int, int>> holeOrder;     // Para almacenar el orden de selección de celdas para los agujeros

        ImGui::Begin("Checker Matrix");

        // Desplegable para la matriz de foot prints
        if (ImGui::CollapsingHeader("Foot Prints Matrix", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Foot Prints:");

            // Generar la matriz de checkboxes
            for (int row = 0; row < xAxis; ++row) {
                for (int col = 0; col < yAxis; ++col) {
                    ImGui::PushID(row * yAxis + col); // Asegurar identificadores únicos

                    bool oldValue = checkerMatrix[row][col];
                    if (ImGui::Checkbox("", &checkerMatrix[row][col])) {
                        if (checkerMatrix[row][col]) {
                            // Añadir celda a la lista de selección si se marca
                            selectedOrder.emplace_back(row, col);
                        }
                        else {
                            // Eliminar celda de la lista de selección si se desmarca
                            auto it = std::remove(selectedOrder.begin(), selectedOrder.end(), std::make_pair(row, col));
                            selectedOrder.erase(it, selectedOrder.end());
                        }
                    }
                    ImGui::PopID();

                    if (col < yAxis - 1) ImGui::SameLine(); // Mantener los checkboxes en la misma línea
                }
            }
        }

        // Desplegable para la matriz de holes
        if (ImGui::CollapsingHeader("Holes Matrix", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Holes:");

            // Generar la matriz de checkboxes para los agujeros
            for (int row = 0; row < xAxis; ++row) {
                for (int col = 0; col < yAxis; ++col) {
                    ImGui::PushID((row * yAxis + col) + (xAxis * yAxis)); // Asegurar identificadores únicos

                    bool oldValue = holeMatrix[row][col];
                    if (ImGui::Checkbox("", &holeMatrix[row][col])) {
                        if (holeMatrix[row][col]) {
                            // Añadir celda a la lista de selección si se marca
                            holeOrder.emplace_back(row, col);
                        }
                        else {
                            // Eliminar celda de la lista de selección si se desmarca
                            auto it = std::remove(holeOrder.begin(), holeOrder.end(), std::make_pair(row, col));
                            holeOrder.erase(it, holeOrder.end());
                        }
                    }
                    ImGui::PopID();

                    if (col < yAxis - 1) ImGui::SameLine(); // Mantener los checkboxes en la misma línea
                }
            }
        }

        // Botón para recoger los resultados
        if (ImGui::Button("Generate Roof")) {
            selectedPoints.clear(); // Limpiar el vector de puntos seleccionados
            for (const auto& cell : selectedOrder) {
                int row = cell.first;
                int col = cell.second;
                if (checkerMatrix[row][col]) {
                    // Convertir la posición de la matriz en coordenadas (x, y)
                    float x = static_cast<float>(col) * scale; // Dividir por scale para escalar
                    float y = static_cast<float>(9 - row) * scale; // Invertir Y para que (0,0) esté en la parte inferior izquierda
                    selectedPoints.push_back(Point{ x, y });
                }
            }

            holePoints.clear(); // Limpiar el vector de puntos de los agujeros
            for (const auto& cell : holeOrder) {
                int row = cell.first;
                int col = cell.second;
                if (holeMatrix[row][col]) {
                    // Convertir la posición de la matriz en coordenadas (x, y)
                    float x = static_cast<float>(col) * scale; // Dividir por scale para escalar
                    float y = static_cast<float>(9 - row) * scale; // Invertir Y para que (0,0) esté en la parte inferior izquierda
                    holePoints.push_back(Point{ x, y });
                }
            }

            // Llamar al callback con los puntos seleccionados
            if (m_callbackFromGuiLayerFunc) {
                std::vector<Vector2d> vector2dPoints;
                std::vector<Vector2d> vector2dHolePoints;
                for (const auto& point : selectedPoints) {
                    vector2dPoints.push_back(Vector2d(point.x, point.y));
                }
                for (const auto& point : holePoints) {
                    vector2dHolePoints.push_back(Vector2d(point.x, point.y));
                }
                m_callbackFromGuiLayerFunc(vector2dPoints, vector2dHolePoints);
            }
        }

        // Botón para limpiar la matriz y la lista de puntos seleccionados
        if (ImGui::Button("Clear")) {
            selectedPoints.clear();
            holePoints.clear();
            selectedOrder.clear();
            holeOrder.clear();
            std::fill(&checkerMatrix[0][0], &checkerMatrix[0][0] + sizeof(checkerMatrix), false);
            std::fill(&holeMatrix[0][0], &holeMatrix[0][0] + sizeof(holeMatrix), false);
        }

        // Mostrar los puntos seleccionados en el formato requerido con índice
        ImGui::Text("Selected Points:");
        for (size_t i = 0; i < selectedPoints.size(); ++i) {
            const auto& point = selectedPoints[i];
            ImGui::Text("%zu: { %.1f, %.1f },", i, point.x, point.y);
        }

        // Mostrar los puntos de los agujeros en el formato requerido con índice
        ImGui::Text("Hole Points:");
        for (size_t i = 0; i < holePoints.size(); ++i) {
            const auto& point = holePoints[i];
            ImGui::Text("%zu: { %.1f, %.1f },", i, point.x, point.y);
        }

        ImGui::End();
    }
    //Aqui llega la funcion para que el editor de Roofs devuelva a quien sea, la matriz de footPrints
    void GuiLayer::SetCallBackFunc(CallbackFromGuiLayer callbackFromGuiLayerFunc)
    {
        m_callbackFromGuiLayerFunc = callbackFromGuiLayerFunc;
    }
    //-----------------------------------------------------------------------------------------------------


    void GuiLayer::end()
    {
        ImGui::End(); // End of Docking Example window

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(m_window_W, m_window_H);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
    void printMatrix(const glm::mat4& mat, const std::string& name) {
        const float* m = glm::value_ptr(mat);
        std::cout << name << " Matrix:" << std::endl;
        std::cout << m[0] << ", " << m[1] << ", " << m[2] << ", " << m[3] << std::endl;
        std::cout << m[4] << ", " << m[5] << ", " << m[6] << ", " << m[7] << std::endl;
        std::cout << m[8] << ", " << m[9] << ", " << m[10] << ", " << m[11] << std::endl;
        std::cout << m[12] << ", " << m[13] << ", " << m[14] << ", " << m[15] << std::endl;
    }
}