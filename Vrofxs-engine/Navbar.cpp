

#include "Navbar.h"


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