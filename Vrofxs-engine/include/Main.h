#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Shader.h"
#include "Camera.h"
#include "Cube.h"
#include "Axis.h"
#include "Main.h"
#include "Navbar.h"
#include "GridCalc.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

extern glm::vec3 cubePosition;
extern glm::vec3 backgroundColor;
extern Camera camera;
extern float deltaTime;
extern Shader gridShader;

void renderGrid(int size, float spacing);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderInspector();
bool checkCubeClick(double xpos, double ypos);
void setupPostProcessing();
void renderPostProcessing();
void renderDirectScene(Shader& screenShader);
void renderNavbar();

