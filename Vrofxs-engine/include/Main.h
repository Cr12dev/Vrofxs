#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Shader.h"
#include "Camera.h"
#include "Cube.h"
#include "Axis.h"
#include "Navbar.h"
#include "GridCalc.h"
#include "Sun.h"
#include "Light.h"
#include "Objects.h"

// Forward declaration
class SceneObject;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

extern glm::vec3 cubePosition;
extern glm::vec3 backgroundColor;
extern Camera camera;
extern float deltaTime;
extern Shader gridShader;
extern bool showGrid;

void renderGrid(int size, float spacing);
void renderHierarchy();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderInspector();
void renderProperties();
bool checkCubeClick(double xpos, double ypos);
bool checkCubeClickOnObject(double xpos, double ypos, SceneObject* obj);
void setupPostProcessing();
void renderPostProcessing();
void renderDirectScene(Shader& screenShader);
void renderNavbar();

// Shadow mapping
extern unsigned int shadowMapFBO;
extern unsigned int shadowMapTexture;
extern const unsigned int SHADOW_WIDTH;
extern const unsigned int SHADOW_HEIGHT;
void setupShadowMap();
glm::mat4 getLightSpaceMatrix();

