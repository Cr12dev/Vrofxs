#include "include/Main.h"



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
bool axisVisible = false;

glm::vec3 cubePosition(0.0f, 0.0f, 0.0f);
glm::vec3 backgroundColor(0.2f, 0.4f, 0.8f);
Shader gridShader; // Declare but don't initialize yet
bool showGrid = true; // Show grid by default

// Variables para post-procesamiento
unsigned int framebuffer, textureColorbuffer, rbo;
unsigned int quadVAO, quadVBO;

// Variables para shadow mapping
unsigned int shadowMapFBO;
unsigned int shadowMapTexture;
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

// Efectos de post-procesamiento
bool postProcessEnabled = true;
bool invertEffect = false;
bool grayscaleEffect = false;
bool edgeEffect = false;

// Cubo estático global para renderizado - se inicializa después de OpenGL
static Cube* g_renderCube = nullptr;

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

    // Inicializar cubo estático DESPUÉS de que OpenGL esté listo
    g_renderCube = new Cube(glm::vec3(0,0,0));

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

    Shader cubeShader("shaders/cube_light.vert", "shaders/cube_light.frag");
    Shader axisShader("shaders/axis.vert", "shaders/axis.frag");
    Shader screenShader("shaders/screen.vert", "shaders/screen.frag");
    Shader sunShader("shaders/sun.vert", "shaders/sun.frag");
    Shader lightShader("shaders/cube_light.vert", "shaders/cube_light.frag");
    gridShader = Shader("shaders/grid.vert", "shaders/grid.frag");
    
    // Check if grid shader compiled successfully
    if (!gridShader.ID) {
        std::cout << "ERROR: Grid shader failed to compile!" << std::endl;
        std::cout << "Please check grid.vert and grid.frag files for syntax errors." << std::endl;
    } else {
        std::cout << "SUCCESS: Grid shader loaded successfully!" << std::endl;
    }
    
    // Initialize grid with safe default values
    gridVAO = 0;
    gridVBO = 0;

    // Cube cube(cubePosition);  // Comentado: usar SceneManager para crear objetos
    Axis axis;
    Sun sun(glm::vec3(0.0f, 10.0f, -5.0f), glm::vec3(1.0f, 0.9f, 0.0f), 1.0f);

    // Espera antes de configurar post-procesamiento (operación sospechosa)
    glfwWaitEventsTimeout(200);

    // Configurar post-procesamiento con más retraso
    setupPostProcessing();

    // Configurar shadow mapping
    setupShadowMap();

    // Obtener dimensiones reales de la ventana después de maximizar
    int actualWidth, actualHeight;
    glfwGetWindowSize(window, &actualWidth, &actualHeight);

    // Actualizar viewport con dimensiones reales
    glViewport(0, 0, actualWidth, actualHeight);

    // Actualizar proyección con dimensiones reales
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)actualWidth / (float)actualHeight, 0.1f, 100.0f);

    Shader depthShader("shaders/shadow_depth.vert", "shaders/shadow_depth.frag");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // ===== PRIMER PASO: RENDERIZAR SHADOW MAP (desde perspectiva de la luz) =====
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        
        depthShader.use();
        
        glm::mat4 lightSpaceMatrix = getLightSpaceMatrix();
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        // depthShader.setMat4("model", cube.GetModelMatrix());
        // cube.Draw(depthShader, camera);
        
        // Restaurar viewport normal
        int actualWidth, actualHeight;
        glfwGetWindowSize(window, &actualWidth, &actualHeight);
        glViewport(0, 0, actualWidth, actualHeight);
        
        // ===== SEGUNDO PASO: RENDERIZAR ESCENA NORMAL CON SOMBRAS =====
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // Renderizar a framebuffer (FONDO ESCENARIO)
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //Negro default
        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f); //Color dinámico
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 view = camera.GetViewMatrix();



        // Renderizar cubo con iluminación y sombras
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        lightShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        // cube.SetPosition(cubePosition);
        // lightShader.setMat4("model", cube.GetModelMatrix());
        lightShader.setVec3("lightPos", glm::vec3(2.0f, 4.0f, 2.0f));
        lightShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        lightShader.setVec3("lightDir", glm::vec3(0.0f, -1.0f, 0.0f));
        lightShader.setVec3("objectColor", glm::vec3(0.5f, 0.3f, 0.8f));
        lightShader.setVec3("viewPos", camera.Position);
        lightShader.setFloat("shininess", 32.0f);
        lightShader.setFloat("ambient", 0.3f);
        lightShader.setFloat("diffuse", 0.8f);
        lightShader.setFloat("specular", 0.5f);
        lightShader.setFloat("shadowBias", 0.005f);
        lightShader.setInt("pcfSamples", 2);
        
        // Activar shadow map en texture unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
        lightShader.setInt("shadowMap", 1);
        
        // cube.Draw(lightShader, camera);

        // Renderizar objetos del SceneManager
        auto& manager = SceneManager::GetInstance();
        
        if (g_renderCube) {
            for (auto& obj : manager.objects) {
                if (!obj->visible) continue;
                
                if (obj->type == ObjectType::Cube) {
                    // Aplicar matriz de transformación del objeto
                    glm::mat4 modelMatrix = obj->GetWorldMatrix();
                    lightShader.setMat4("model", modelMatrix);
                    // Usar mesh directamente para no sobrescribir la matriz
                    if (g_renderCube->mesh) {
                        g_renderCube->mesh->Draw();
                    }
                }
            }
        }

        // Renderizar ejes cartesianos en la posición del objeto seleccionado
        SceneObject* selected = SceneManager::GetInstance().GetSelected();
        if (selected && axisVisible) {
            axisShader.use();
            axisShader.setMat4("projection", projection);
            axisShader.setMat4("view", view);
            glm::mat4 axisModel = glm::mat4(1.0f);
            axisModel = glm::translate(axisModel, selected->GetWorldPosition());
            axisShader.setMat4("model", axisModel);
            axis.Draw(view, projection);
        }

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
        renderHierarchy();
        renderNavbar();
        renderInspector();
        renderProperties();
        
        // Only render grid if enabled
        if (showGrid) {
            renderGrid(size = 20, spacing = 1.0f);
        }

        float sunDistance = 50.0f; // distancia fija del sol
        glm::vec3 sunDirection = glm::normalize(glm::vec3(0.3f, 1.0f, 0.2f)); // dirección del sol
        sun.updatePosition(camera.Position + sunDirection * sunDistance);
        
        // Renderizar el sol
        sun.render(
            sunShader,
            camera.GetViewMatrix(),
            projection,
            camera.Position
        );
        

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
    float cameraRotationSpeed = 100.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        camera.ProcessMouseMovement(-cameraRotationSpeed, 0.0f, true);  // Izquierda
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        camera.ProcessMouseMovement(cameraRotationSpeed, 0.0f, true);   // Derecha
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        camera.ProcessMouseMovement(0.0f, -cameraRotationSpeed, true);  // Arriba
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        camera.ProcessMouseMovement(0.0f, cameraRotationSpeed, true);   // Abajo
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

        // Mover el objeto seleccionado del SceneManager
        SceneObject* selected = SceneManager::GetInstance().GetSelected();
        if (selected) {
            selected->transform.position.x += xoffset * 0.01f;
            selected->transform.position.y += yoffset * 0.01f;
        }

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

        camera.ProcessMouseMovement(xoffset, yoffset, true);

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

            // Verificar si se hizo clic en algun objeto
            SceneObject* clicked = nullptr;
            auto& manager = SceneManager::GetInstance();
            for (auto& obj : manager.objects) {
                if (obj->type == ObjectType::Cube && checkCubeClickOnObject(xpos, ypos, obj.get())) {
                    clicked = obj.get();
                    break;
                }
            }
            
            if (clicked) {
                manager.SelectObject(clicked);
                axisVisible = true;
                inspectorVisible = true;
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

bool checkCubeClickOnObject(double xpos, double ypos, SceneObject* obj) {
    if (!obj) return false;
    
    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    
    float clickThreshold = 200.0f;
    glm::vec3 pos = obj->GetWorldPosition();
    
    glm::vec4 screenPos = glm::vec4(pos, 1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
    
    screenPos = projection * view * screenPos;
    
    if (screenPos.w != 0.0f) {
        screenPos.x /= screenPos.w;
        screenPos.y /= screenPos.w;
    }
    
    float screenX = (screenPos.x + 1.0f) * width / 2.0f;
    float screenY = (1.0f - screenPos.y) * height / 2.0f;
    
    float distance = sqrt(pow(xpos - screenX, 2) + pow(ypos - screenY, 2));
    
    return distance < clickThreshold;
}

/* --- Renderizar jerarquía de objetos ---*/
void renderHierarchy() {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
    
    if (ImGui::Begin("Hierarchy", nullptr, flags)) {
        // Botones de acción
        
        if (ImGui::CollapsingHeader("Create a object")) {
            if (ImGui::Button("Create Empty")) {
                static int emptyCount = 0;
                SceneManager::GetInstance().CreateEmpty("Empty" + std::to_string(emptyCount++));
            }
            if (ImGui::Button("Create Cube")) {
                static int cubeCount = 0;
                auto newCube = SceneManager::GetInstance().CreateCube("Cube" + std::to_string(cubeCount++), glm::vec3(0, 0, 0));
                // Seleccionar automáticamente y mostrar ejes
                SceneManager::GetInstance().SelectObject(newCube.get());
                axisVisible = true;
                cubePosition = newCube->transform.position;
            }
        }
        
        ImGui::Separator();
        if (ImGui::Button("Delete")) {
            SceneManager::GetInstance().DeleteSelected();
        }
        
        ImGui::Separator();
        
        // Lista de objetos
        auto& manager = SceneManager::GetInstance();
        ImGui::Text("Objects (%d):", manager.GetObjectCount());
        ImGui::Spacing();
        
        for (auto& obj : manager.objects) {
            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            
            if (obj->selected) {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }
            
            // Icono según tipo
            const char* icon = "?";
            switch (obj->type) {
                case ObjectType::Cube: icon = "■"; break;
                case ObjectType::Empty: icon = "○"; break;
                default: icon = "?"; break;
            }
            
            std::string label = std::string(icon) + " " + obj->name;
            
            bool isOpen = ImGui::TreeNodeEx(label.c_str(), nodeFlags);
            
            // Click para seleccionar
            if (ImGui::IsItemClicked()) {
                manager.SelectObject(obj.get());
                axisVisible = true;  // Mostrar ejes al seleccionar
                inspectorVisible = true;
                // Sincronizar con posición del cubo actual
                cubePosition = obj->transform.position;
            }
        }
        
        // Si hay objeto seleccionado, mostrar info
        SceneObject* selected = manager.GetSelected();
        if (selected) {
            ImGui::Separator();
            ImGui::Text("Selected: %s", selected->name.c_str());
            ImGui::Text("Type: %d", (int)selected->type);
            ImGui::Text("Position: %.2f, %.2f, %.2f", 
                selected->transform.position.x,
                selected->transform.position.y,
                selected->transform.position.z);
        }
    }
    ImGui::End();
}
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
        SceneObject* selected = SceneManager::GetInstance().GetSelected();
        
        if (selected) {
            ImGui::Text("Selected: %s", selected->name.c_str());
            ImGui::Separator();
            ImGui::Text("Position:");

            // Mostrar coordenadas X, Y, Z del objeto seleccionado
            ImGui::PushItemWidth(200);

            float xVal = selected->transform.position.x;
            float yVal = selected->transform.position.y;
            float zVal = selected->transform.position.z;

            ImGui::Text("X:");
            ImGui::SameLine();
            if (ImGui::DragFloat("##X", &xVal, 0.1f)) {
                selected->transform.position.x = xVal;
            }

            ImGui::Text("Y:");
            ImGui::SameLine();
            if (ImGui::DragFloat("##Y", &yVal, 0.1f)) {
                selected->transform.position.y = yVal;
            }

            ImGui::Text("Z:");
            ImGui::SameLine();
            if (ImGui::DragFloat("##Z", &zVal, 0.1f)) {
                selected->transform.position.z = zVal;
            }

            ImGui::PopItemWidth();

            ImGui::Separator();

            // Botón para resetear posición
            if (ImGui::Button("Reset Position")) {
                selected->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
            }
        } else {
            ImGui::Text("No object selected");
            ImGui::Text("Click 'Create Cube' to add an object");
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

    // Shader estático - solo se crea una vez
    static Shader screenShader("shaders/screen.vert", "shaders/screen.frag");
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


void setupShadowMap()
{
    // Crear framebuffer para shadow map
    glGenFramebuffers(1, &shadowMapFBO);
    
    // Crear textura de profundidad
    glGenTextures(1, &shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    // Adjuntar textura de profundidad al framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::SHADOWMAP:: Framebuffer no esta completo!" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 getLightSpaceMatrix()
{
    // Configurar matrices para renderizar desde la perspectiva de la luz
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
    glm::vec3 lightPos = glm::vec3(2.0f, 8.0f, 2.0f);
    glm::vec3 lightTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    return lightProjection * lightView;
}
