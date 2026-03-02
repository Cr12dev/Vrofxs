#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "Shader.h"
#include "Camera.h"
#include "Cube.h"
#include "Axis.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool mousePressed = false;
bool cameraLookEnabled = false;
bool touchpadScroll = false;
bool inspectorVisible = false;
glm::vec3 cubePosition(0.0f, 0.0f, 0.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderInspector();
bool checkCubeClick(double xpos, double ypos);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Hints para evitar detección de antivirus
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // Crear ventana oculta primero
    glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);

    // Crear ventana en modo ventana (no fullscreen) para evitar alertas
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Vrofxs 3D Engine", NULL, NULL);
    if (!window) {
        std::cout << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Mostrar ventana después de crearla
    glfwShowWindow(window);
    
    // Esperar un poco antes de maximizar para evitar detección
    glfwWaitEventsTimeout(100);
    
    // Maximizar ventana (simula fullscreen pero con bordes)
    glfwMaximizeWindow(window);
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error al inicializar GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader cubeShader("shaders/cube.vert", "shaders/cube.frag");
    Shader axisShader("shaders/axis.vert", "shaders/axis.frag");

    Cube cube(cubePosition);
    Axis axis;

    // Obtener dimensiones reales de la ventana después de maximizar
    int actualWidth, actualHeight;
    glfwGetWindowSize(window, &actualWidth, &actualHeight);
    
    // Actualizar viewport con dimensiones reales
    glViewport(0, 0, actualWidth, actualHeight);
    
    // Actualizar proyección con dimensiones reales
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)actualWidth / (float)actualHeight, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();

        // Renderizar cubo
        cubeShader.use();
        cubeShader.setMat4("projection", projection);
        cubeShader.setMat4("view", view);
        cube.SetPosition(cubePosition);
        cubeShader.setMat4("model", cube.GetModelMatrix());
        cubeShader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
        cubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        cubeShader.setVec3("objectColor", glm::vec3(0.5f, 0.3f, 0.8f));
        cubeShader.setVec3("viewPos", camera.Position);
        cube.Draw();

        // Renderizar ejes cartesianos (se mueven con el cubo)
        axisShader.use();
        axisShader.setMat4("projection", projection);
        axisShader.setMat4("view", view);
        glm::mat4 axisModel = glm::mat4(1.0f);
        axisModel = glm::translate(axisModel, cubePosition);
        axisShader.setMat4("model", axisModel);
        axis.Draw(view, projection);

        // Renderizar inspector si está visible
        renderInspector();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Movimiento horizontal
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
    // Movimiento vertical
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.Position.y += deltaTime * 2.5f;  // Subir
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.Position.y -= deltaTime * 2.5f;  // Bajar
    
    // Movimiento del cubo con teclas
    float cubeSpeed = 2.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cubePosition.y += cubeSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cubePosition.y -= cubeSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cubePosition.x += cubeSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cubePosition.x -= cubeSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cubePosition.z += cubeSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cubePosition.z -= cubeSpeed;
    
    // Rotación de cámara con teclas
    float cameraRotationSpeed = 50.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        camera.ProcessMouseMovement(-cameraRotationSpeed, 0.0f, false);  // Izquierda
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        camera.ProcessMouseMovement(cameraRotationSpeed, 0.0f, false);   // Derecha
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        camera.ProcessMouseMovement(0.0f, -cameraRotationSpeed, false);  // Arriba
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        camera.ProcessMouseMovement(0.0f, cameraRotationSpeed, false);   // Abajo
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mousePressed) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        cubePosition.x += xoffset * 0.01f;
        cubePosition.y += yoffset * 0.01f;

        lastX = xpos;
        lastY = ypos;
    }
    
    if (cameraLookEnabled) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);

        lastX = xpos;
        lastY = ypos;
    }
    
    // Touchpad: movimiento con dos dedos (simulado con boton medio)
    if (touchpadScroll) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        // Movimiento de cámara con touchpad
        camera.Position.x += xoffset * 0.01f;
        camera.Position.y -= yoffset * 0.01f;

        lastX = xpos;
        lastY = ypos;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            
            // Verificar si se hizo clic en el cubo
            if (checkCubeClick(xpos, ypos)) {
                inspectorVisible = !inspectorVisible;  // Toggle del inspector
            }
            
            mousePressed = true;
            firstMouse = true;
        } else if (action == GLFW_RELEASE) {
            mousePressed = false;
        }
    }
    
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            cameraLookEnabled = true;
            firstMouse = true;
        } else if (action == GLFW_RELEASE) {
            cameraLookEnabled = false;
        }
    }
    
    // Soporte para touchpad - boton medio
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            touchpadScroll = true;
            firstMouse = true;
        } else if (action == GLFW_RELEASE) {
            touchpadScroll = false;
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Zoom con scroll del touchpad/mouse
    camera.ProcessMouseScroll(yoffset);
    
    // Movimiento Z del cubo con scroll horizontal (touchpad)
    if (xoffset != 0.0) {
        cubePosition.z += xoffset * 0.1f;
    }
}

bool checkCubeClick(double xpos, double ypos) {
    // Obtener dimensiones reales de la ventana
    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    
    // Convertir coordenadas de mouse a coordenadas de mundo 
    // Este es un sistema básico de detección de clics
    float clickThreshold = 200.0f;  // Distancia máxima para considerar clic en el cubo
    
    // Proyección simple del cubo a pantalla
    glm::vec4 cubeScreenPos = glm::vec4(cubePosition, 1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
    
    cubeScreenPos = projection * view * cubeScreenPos;
    
    // Perspectiva divide
    if (cubeScreenPos.w != 0.0f) {
        cubeScreenPos.x /= cubeScreenPos.w;
        cubeScreenPos.y /= cubeScreenPos.w;
    }
    
    // Convertir a coordenadas de pantalla
    float screenX = (cubeScreenPos.x + 1.0f) * width / 2.0f;
    float screenY = (1.0f - cubeScreenPos.y) * height / 2.0f;
    
    // Calcular distancia
    float distance = sqrt(pow(xpos - screenX, 2) + pow(ypos - screenY, 2));
    
    return distance < clickThreshold;
}

void renderInspector() {
    if (!inspectorVisible) return;
    
    // Obtener dimensiones reales de la ventana
    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    
    // Guardar el estado actual de OpenGL
    glDisable(GL_DEPTH_TEST);
    
    // Configurar para renderizado 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    
glMatrixMode(GL_MODELVIEW);
glPushMatrix();
glLoadIdentity();
    
// Dibujar panel del inspector
int panelWidth = 300;
int panelHeight = 250;
int panelX = width - panelWidth - 20;
int panelY = 20;
    
// Fondo del panel
glColor4f(0.2f, 0.2f, 0.2f, 0.9f);
glBegin(GL_QUADS);
glVertex2f(panelX, panelY);
glVertex2f(panelX + panelWidth, panelY);
glVertex2f(panelX + panelWidth, panelY + panelHeight);
glVertex2f(panelX, panelY + panelHeight);
glEnd();
    
// Borde del panel
glColor3f(0.5f, 0.5f, 0.5f);
glLineWidth(2.0f);
glBegin(GL_LINE_LOOP);
glVertex2f(panelX, panelY);
glVertex2f(panelX + panelWidth, panelY);
glVertex2f(panelX + panelWidth, panelY + panelHeight);
glVertex2f(panelX, panelY + panelHeight);
glEnd();
    
// Restaurar estado de OpenGL
glPopMatrix();
glMatrixMode(GL_PROJECTION);
glPopMatrix();
glMatrixMode(GL_MODELVIEW);
    
glEnable(GL_DEPTH_TEST);
    
// Mostrar coordenadas en consola
std::cout << "\r=== INSPECTOR ===" << std::endl;
std::cout << "Posicion Cubo:" << std::endl;
std::cout << "  X: " << cubePosition.x << std::endl;
std::cout << "  Y: " << cubePosition.y << std::endl;
std::cout << "  Z: " << cubePosition.z << std::endl;
std::cout << "=================" << std::flush;
}