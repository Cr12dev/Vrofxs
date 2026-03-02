#include "Axis.h"

Axis::Axis() {
    setupAxis();
}

void Axis::setupAxis() {
    float axisVertices[] = {
        // X axis - Red
        0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        2.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        
        // Y axis - Green
        0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.0f, 2.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        
        // Z axis - Blue
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 2.0f,  0.0f, 0.0f, 1.0f
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    
    glBindVertexArray(0);
}

void Axis::Draw(glm::mat4 view, glm::mat4 projection) {
    glm::mat4 model = glm::mat4(1.0f);
    
    glBindVertexArray(VAO);
    
    glLineWidth(3.0f);
    
    for (int i = 0; i < 3; i++) {
        glDrawArrays(GL_LINES, i * 2, 2);
    }
    
    glBindVertexArray(0);
}
