#include "Main.h"



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
glm::vec3 backgroundColor(0.2f, 0.4f, 0.8f);
Shader gridShader("shaders/grid.vert", "shaders/grid.frag");

// Variables para post-procesamiento
unsigned int framebuffer, textureColorbuffer, rbo;
unsigned int quadVAO, quadVBO;

// Efectos de post-procesamiento
bool postProcessEnabled = true;
bool invertEffect = false;
bool grayscaleEffect = false;
bool edgeEffect = false;


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // Crear ventana con tamaño más estándar
    GLFWwindow* window = glfwCreateWindow(1024, 768, "3D Application", NULL, NULL);
    if (!window) {
        std::cout << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Espera prolongada antes de mostrar
    glfwWaitEventsTimeout(200);

    // Mostrar ventana gradualmente
    glfwShowWindow(window);
    glfwWaitEventsTimeout(150);

    // Maximizar suavemente
    glfwMaximizeWindow(window);
    glfwWaitEventsTimeout(100);

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

    // Esperas adicionales antes de inicializar componentes complejos
    glfwWaitEventsTimeout(100);

    // Inicializar ImGui con más retraso
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Shader cubeShader("shaders/cube.vert", "shaders/cube.frag");
    Shader axisShader("shaders/axis.vert", "shaders/axis.frag");
    Shader screenShader("shaders/screen.vert", "shaders/screen.frag");

    Cube cube(cubePosition);
    Axis axis;

    // Espera antes de configurar post-procesamiento (operación sospechosa)
    glfwWaitEventsTimeout(200);

    // Configurar post-procesamiento con más retraso
    setupPostProcessing();

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

        // Renderizar a framebuffer (FONDO ESCENARIO)
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //Negro default
        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f); //Color dinámico
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

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

        // Volver al framebuffer por defecto
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(backgroundColor.r * 0.8f, backgroundColor.g * 0.8f, backgroundColor.b * 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Renderizar post-procesamiento o escena directa
        if (postProcessEnabled) {
            renderPostProcessing();
        }
        else {
            // Renderizar escena directamente sin post-procesamiento
            renderDirectScene(screenShader);
        }

        // Iniciar nuevo frame de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Renderizar inspector con ImGui
        int size;
        float spacing;
        renderNavbar();
        renderInspector();
        renderProperties();
        renderGrid(size = 1, spacing = 1.0f);
        

        // Renderizar ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpiar ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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
        }
        else if (action == GLFW_RELEASE) {
            mousePressed = false;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            cameraLookEnabled = true;
            firstMouse = true;
        }
        else if (action == GLFW_RELEASE) {
            cameraLookEnabled = false;
        }
    }

    // Soporte para touchpad - boton medio
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            touchpadScroll = true;
            firstMouse = true;
        }
        else if (action == GLFW_RELEASE) {
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

/* --- Renderizar panel inspector de objeto ---*/
void renderInspector() {
    if (!inspectorVisible) return;

    int windowWidth = SCR_WIDTH;
    int windowheight = SCR_HEIGHT;

    // Configurar ventana del inspector
    ImGui::SetNextWindowPos(ImVec2(windowWidth*3-100, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, windowheight*3-10), ImGuiCond_Always);

    // ImGui flags
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove;

    // Crear ventana del inspector
    if (ImGui::Begin("Inspector", &inspectorVisible, flags)) {
        ImGui::Text("Posicion del Cubo:");

        // Mostrar coordenadas X, Y, Z
        ImGui::PushItemWidth(200);

        // Coordenada X
        ImGui::Text("X:");
        ImGui::SameLine();
        float xVal = cubePosition.x;
        if (ImGui::DragFloat("##X", &xVal, 0.1f)) {
            cubePosition.x = xVal;
        }

        // Coordenada Y
        ImGui::Text("Y:");
        ImGui::SameLine();
        float yVal = cubePosition.y;
        if (ImGui::DragFloat("##Y", &yVal, 0.1f)) {
            cubePosition.y = yVal;
        }

        // Coordenada Z
        ImGui::Text("Z:");
        ImGui::SameLine();
        float zVal = cubePosition.z;
        if (ImGui::DragFloat("##Z", &zVal, 0.1f)) {
            cubePosition.z = zVal;
        }

        ImGui::PopItemWidth();

        ImGui::Separator();

        // Mostrar valores actuales
        ImGui::Text("Valores Actuales:");
        ImGui::Text("X: %.2f", cubePosition.x);
        ImGui::Text("Y: %.2f", cubePosition.y);
        ImGui::Text("Z: %.2f", cubePosition.z);

        // Botón para resetear posición
        if (ImGui::Button("Resetear Posicion")) {
            cubePosition = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        ImGui::Separator();

        // Controles de post-procesamiento
        ImGui::Text("Post-Procesamiento:");
        ImGui::Checkbox("Activar Post-Procesamiento", &postProcessEnabled);

        if (postProcessEnabled) {
            ImGui::Checkbox("Invertir Colores", &invertEffect);
            ImGui::Checkbox("Escala de Grises", &grayscaleEffect);
            ImGui::Checkbox("Detección de Bordes", &edgeEffect);
        }
    }
    ImGui::End();

    // También mostrar en consola para compatibilidad
    std::cout << "\r=== INSPECTOR ===" << std::endl;
    std::cout << "Posicion Cubo:" << std::endl;
    std::cout << "  X: " << cubePosition.x << std::endl;
    std::cout << "  Y: " << cubePosition.y << std::endl;
    std::cout << "  Z: " << cubePosition.z << std::endl;
    std::cout << "=================" << std::flush;
}

void setupPostProcessing() {
    // Obtener dimensiones reales de la ventana
    int actualWidth, actualHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &actualWidth, &actualHeight);

    // Configurar framebuffer para post-procesamiento
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Crear textura de color con dimensiones reales
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, actualWidth, actualHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // Crear renderbuffer para depth/stencil con dimensiones reales
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, actualWidth, actualHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer no está completo!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Configurar quad para renderizado del post-procesamiento
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void renderPostProcessing() {
    if (!postProcessEnabled) return;

    // Usar shader de post-procesamiento
    Shader screenShader("shaders/screen.vert", "shaders/screen.frag");
    screenShader.use();

    // Configurar uniforms para efectos
    screenShader.setFloat("time", glfwGetTime());
    screenShader.setBool("invert", invertEffect);
    screenShader.setBool("grayscale", grayscaleEffect);
    screenShader.setBool("edge", edgeEffect);

    // Renderizar quad con textura del framebuffer
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void renderDirectScene(Shader& screenShader)
{
    screenShader.use();

    // Desactivar todos los efectos
    screenShader.setBool("invert", false);
    screenShader.setBool("grayscale", false);
    screenShader.setBool("edge", false);

    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

    screenShader.setInt("screenTexture", 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

