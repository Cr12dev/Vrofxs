#include "include/Navbar.h"

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

            ImGui::Checkbox("Mostrar Grid", &showGrid);

            ImGui::ColorEdit3("Color de Fondo", (float*)&backgroundColor);
        }

        ImGui::Separator();

        // Sección de Controles de Cámara
        if (ImGui::CollapsingHeader("Camera Controls")) {
            ImGui::Text("Controles de cámara:");
            
            ImGui::Text("Movimiento:");
            ImGui::BulletText("W/S - Adelante/Atrás");
            ImGui::BulletText("A/D - Izquierda/Derecha");
            ImGui::BulletText("Espacio/Shift - Subir/Bajar");
            
            ImGui::Text("Rotación:");
            ImGui::BulletText("Botón derecho + arrastrar - Rotar vista");
            ImGui::BulletText("J/L - Rotar izquierda/derecha");
            ImGui::BulletText("I/K - Rotar arriba/abajo");
            
            ImGui::Text("Zoom:");
            ImGui::BulletText("Scroll del ratón - Acercar/alejar");
            
            // Mostrar posición actual de la cámara
            ImGui::Separator();
            ImGui::Text("Posición Cámara:");
            ImGui::Text("X: %.2f", camera.Position.x);
            ImGui::Text("Y: %.2f", camera.Position.y);
            ImGui::Text("Z: %.2f", camera.Position.z);
        }

        ImGui::Separator();

        

    ImGui::End();
    }
}