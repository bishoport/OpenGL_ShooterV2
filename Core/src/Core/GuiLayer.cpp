#include "GuiLayer.h"
#include "../tools/ShaderManager.h"
#include <imgui_internal.h>

namespace libCore
{
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
    }

    //Aqui llega la funcion para que el editor de Roofs devuelva a quien sea, la matriz de footPrints
    void libCore::GuiLayer::SetCallBackFunc(CallbackFromGuiLayer callbackFromGuiLayerFunc)
    {
        m_callbackFromGuiLayerFunc = callbackFromGuiLayerFunc;
    }

    GuiLayer::~GuiLayer() {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void GuiLayer::begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void GuiLayer::renderDockers()
    {
        // Configuración del Docking
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
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

    void GuiLayer::renderMainMenuBar() {

        // Barra de menu principal
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open", "Ctrl+O")) { /* Abrir archivo */ }
                if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Guardar archivo */ }
                //if (ImGui::MenuItem("Exit", "Ctrl+Q")) { glfwSetWindowShouldClose(window, true); }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) { /* Deshacer */ }
                if (ImGui::MenuItem("Redo", "Ctrl+Y")) { /* Rehacer */ }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void GuiLayer::DrawViewports(std::vector<Ref<Viewport>> viewports)
    {
        for (auto& viewport : viewports)
        {
            std::string windowTitle = viewport->viewportName;
            ImGui::Begin(windowTitle.c_str(), nullptr);

            // Obtener los tamaños y posiciones actuales de la ventana
            ImVec2 currentViewportSize = ImGui::GetWindowSize();
            ImVec2 currentViewportPos = ImGui::GetWindowPos();

            // Verificar si se está arrastrando la ventana
            bool isCurrentlyDragging = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && ImGui::IsMouseDragging(ImGuiMouseButton_Left);

            // Verificar si el tamaño ha cambiado
            bool isCurrentlyResizing = currentViewportSize.x != viewport->previousViewportSize.x || currentViewportSize.y != viewport->previousViewportSize.y;

            // Detectar cuándo se comienza a mover o redimensionar
            if (!viewport->isMoving && isCurrentlyDragging)
            {
                viewport->isMoving = true;
            }
            if (!viewport->isResizing && isCurrentlyResizing)
            {
                viewport->isResizing = true;
            }

            // Detectar cuándo termina el movimiento o el redimensionamiento
            if (viewport->isMoving && !isCurrentlyDragging)
            {
                viewport->isMoving = false;
                EventManager::OnPanelResizedEvent().trigger(viewport->viewportName, glm::vec2(currentViewportSize.x, currentViewportSize.y),
                    glm::vec2(currentViewportPos.x, currentViewportPos.y));
            }
            if (viewport->isResizing && !isCurrentlyResizing)
            {
                viewport->isResizing = false;
                EventManager::OnPanelResizedEvent().trigger(viewport->viewportName, glm::vec2(currentViewportSize.x, currentViewportSize.y),
                    glm::vec2(currentViewportPos.x, currentViewportPos.y));
            }

            // Actualiza los valores anteriores
            viewport->previousViewportSize = currentViewportSize;
            viewport->previousViewportPos = currentViewportPos;

            // Actualiza los valores actuales en el viewport
            viewport->viewportSize = currentViewportSize;
            viewport->viewportPos = currentViewportPos;

            // Renderiza la imagen
            ImGui::Image((void*)(intptr_t)viewport->framebuffer_final->getTexture("color"), ImVec2(viewport->viewportSize.x, viewport->viewportSize.y), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
            viewport->mouseInviewport = false;
            ImGui::End();
        }
    }

    //PANELES PRINCIPALES
    void GuiLayer::DrawHierarchyPanel(const std::vector<Ref<libCore::ModelContainer>>& modelsInScene)
    {
        ImGui::Begin("Hierarchy");
        for (auto& modelContainer : modelsInScene) {
            if (ImGui::TreeNode(modelContainer.get(), "Model: %s", modelContainer->name.c_str())) {
                for (int j = 0; j < modelContainer->models.size(); j++) {
                    if (ImGui::TreeNode(modelContainer->models[j].get(), "Child: %d", j)) {
                        
                        // Listamos las Meshes de cada Model
                        auto& model = modelContainer->models[j];


                        //MESHES
                        for (int k = 0; k < model->meshes.size(); k++) {
                            ImGui::BulletText("Mesh: %s", model->meshes[k]->meshName.c_str());
                            ImGui::BulletText("Transform:");
                            ImGui::DragFloat3("Position", &model->transform.position[0], 0.1f);
                            ImGui::DragFloat3("Rotation", &model->transform.rotation[0], 0.01f, -3.14159f, 3.14159f, "%.3f rad");
                            ImGui::DragFloat3("Scale", &model->transform.scale[0], 0.1f, 0.01f, 100.0f, "%.3f");
                        }


                        //MATERIALS
                        for (int k = 0; k < model->materials.size(); k++) {
                            auto& material = model->materials[k];

                            if (ImGui::TreeNode(&material, "Material: %s", material->materialName.c_str())) {
                                ImGui::Text("Shader: %s", material->shaderName.c_str());

                                // Manipulate material values
                                ImGui::ColorEdit3("Albedo Color", (float*)&material->albedoColor);
                                ImGui::DragFloat("Normal Strength", &material->normalStrength, 0.1f, -10.0f, 10.0f);
                                ImGui::DragFloat("Metallic Value", &material->metallicValue, 0.1f, 0.0f, 10.0f);
                                ImGui::DragFloat("Roughness Value", &material->roughnessValue, 0.1f, 0.0f, 10.0f);
                                ImGui::DragFloat("AO Value", &material->aoValue, 0.1f, 0.0f, 10.0f);

                                // Display material textures
                                if (material->albedoMap) {
                                    ImGui::Text("Albedo Map");
                                    ImGui::Image((void*)(intptr_t)material->albedoMap->GetTextureID(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
                                }
                                if (material->normalMap) {
                                    ImGui::Text("Normal Map");
                                    ImGui::Image((void*)(intptr_t)material->normalMap->GetTextureID(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
                                }
                                if (material->metallicMap) {
                                    ImGui::Text("Metallic Map");
                                    ImGui::Image((void*)(intptr_t)material->metallicMap->GetTextureID(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
                                }
                                if (material->roughnessMap) {
                                    ImGui::Text("Roughness Map");
                                    ImGui::Image((void*)(intptr_t)material->roughnessMap->GetTextureID(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
                                }
                                if (material->aOMap) {
                                    ImGui::Text("AO Map");
                                    ImGui::Image((void*)(intptr_t)material->aOMap->GetTextureID(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
                                }

                                ImGui::TreePop();
                            }
                        }

                        ImGui::TreePop(); // Finaliza el nodo del Model
                    }
                }
                ImGui::TreePop(); // Finaliza el nodo del ModelContainer
            }
        }
        ImGui::End();
    }
    void GuiLayer::DrawLightsPanel(const std::vector<Ref<libCore::Light>>& lightsInScene)
    {
        ImGui::Begin("Lights");

        for (size_t i = 0; i < lightsInScene.size(); ++i) {
            auto& light = lightsInScene[i];

            // Push a unique identifier onto the ID stack
            ImGui::PushID(static_cast<int>(i));

            ImGui::BulletText("Light: %d", light->id);
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
            static const char* lightTypes[] = { "Point", "Spot", "Area" };
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

            ImGui::Separator();

            // Pop the identifier off the ID stack
            ImGui::PopID();
        }

        ImGui::End();
    }



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
}