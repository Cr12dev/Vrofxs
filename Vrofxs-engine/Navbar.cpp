#include "Navbar.h"

char buffer[128] = "";
bool propertiesVisible = false;

void renderNavbar()
{
    // Inicia la barra de menú principal
    if (ImGui::BeginMainMenuBar())
    {
        // Menú Archivo
        if (ImGui::BeginMenu("Archivo"))
        {
            if (ImGui::MenuItem("Nuevo")) {
                // Acción: resetear cubo por ejemplo
                cubePosition = glm::vec3(0.0f);
            }
            if (ImGui::MenuItem("Abrir")) {
                // Acción: abrir archivo (a implementar)
            }
            if (ImGui::MenuItem("Guardar")) {
                // Acción: guardar escena (a implementar)
            }
            if (ImGui::MenuItem("Properties")) {
                printf("### Executing PROPERTIES ###");
                propertiesVisible = true;
            }
            if (ImGui::MenuItem("Salir")) {
                glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
            }
            ImGui::EndMenu();
        }

        // Menú Editar
        if (ImGui::BeginMenu("Editar"))
        {
            if (ImGui::MenuItem("Copiar")) { /* acción */ }
            if (ImGui::MenuItem("Pegar")) { /* acción */ }
            ImGui::EndMenu();
        }

        // Menú Ayuda
        if (ImGui::BeginMenu("Ayuda"))
        {
            if (ImGui::MenuItem("Acerca de")) {
                std::cout << "3D Engine v0.1" << std::endl;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void renderProperties() {
    if (!propertiesVisible) return;

    int windowWidth = SCR_WIDTH;
    int windowheight = SCR_HEIGHT;

    // Configuracion de la ventana
    ImGui::SetNextWindowPos(ImVec2(99, 100), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(windowWidth*3, windowheight - 100), ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove;

    if (ImGui::Begin("Properties", &propertiesVisible, flags)) {
        ImGui::Text("Environment Properties");
        ImGui::Separator();

        // Sección de Renderizado
        if (ImGui::CollapsingHeader("Render Settings")) {
            ImGui::Text("Opciones de renderizado:");
            
            static bool wireframe = false;
            if (ImGui::Checkbox("Wireframe Mode", &wireframe)) {
                if (wireframe) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }

            static bool showGrid = false;
            ImGui::Checkbox("Mostrar Grid", &showGrid);

            ImGui::ColorEdit3("Color de Fondo", (float*)&backgroundColor);
        }

        ImGui::Separator();

        

    ImGui::End();
    }
}

unsigned int gridVAO, gridVBO;

void renderGrid(int size, float spacing)
{
    // Setup grid if not already done or if parameters changed
    static int lastSize = -1;
    static float lastSpacing = -1.0f;

    if (lastSize != size || lastSpacing != spacing) {
        setupGrid(size, spacing);
        lastSize = size;
        lastSpacing = spacing;
    }

    gridShader.use();
    gridShader.setVec3("lineColor", glm::vec3(0.7f, 0.7f, 0.7f)); // gris

    // Get the current view and projection matrices
    glm::mat4 view = camera.GetViewMatrix();
    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);

    // Set matrices for the grid shader
    gridShader.setMat4("view", view);
    gridShader.setMat4("projection", projection);
    gridShader.setMat4("model", glm::mat4(1.0f)); // Identity matrix for grid at origin

    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);
    glBindVertexArray(0);
}