#pragma once

#include "Main.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// External variables needed by Navbar.cpp
extern glm::vec3 backgroundColor;
extern Camera camera;
extern float deltaTime;

void renderProperties();
void renderNavbar();

