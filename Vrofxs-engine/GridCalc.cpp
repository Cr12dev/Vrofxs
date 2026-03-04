#include "include/GridCalc.h"

std::vector<float> gridVertices;
unsigned int gridVAO, gridVBO;

void setupGrid(int size, float spacing)
{
    gridVertices.clear();

    // Create grid lines like Unity's grid
    for (int i = -size; i <= size; i++)
    {
        // Lines parallel to Z axis (along X)
        gridVertices.push_back(i * spacing); gridVertices.push_back(0); gridVertices.push_back(-size * spacing);
        gridVertices.push_back(i * spacing); gridVertices.push_back(0); gridVertices.push_back(size * spacing);
        
        // Lines parallel to X axis (along Z)  
        gridVertices.push_back(-size * spacing); gridVertices.push_back(0); gridVertices.push_back(i * spacing);
        gridVertices.push_back(size * spacing);  gridVertices.push_back(0); gridVertices.push_back(i * spacing);
    }

    // Create main axis lines (thicker/more prominent)
    // X-axis (red)
    gridVertices.push_back(-size * spacing); gridVertices.push_back(0); gridVertices.push_back(0);
    gridVertices.push_back(size * spacing);  gridVertices.push_back(0); gridVertices.push_back(0);
    
    // Z-axis (blue)
    gridVertices.push_back(0); gridVertices.push_back(0); gridVertices.push_back(-size * spacing);
    gridVertices.push_back(0); gridVertices.push_back(0); gridVertices.push_back(size * spacing);

    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void renderGrid(int size, float spacing)
{
    // Safety checks to prevent access violations
    if (!gridShader.ID) {
        std::cout << "WARNING: Grid shader not loaded, skipping grid render" << std::endl;
        return;
    }
    
    // Setup grid if not already done or if parameters changed
    static int lastSize = -1;
    static float lastSpacing = -1.0f;
    
    if (lastSize != size || lastSpacing != spacing) {
        setupGrid(size, spacing);
        lastSize = size;
        lastSpacing = spacing;
    }
    
    // Additional safety check for VAO
    if (gridVAO == 0) {
        std::cout << "WARNING: Grid VAO not initialized, skipping grid render" << std::endl;
        return;
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
