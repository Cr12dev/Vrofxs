#include "include/Sun.h"

Sun::Sun(glm::vec3 position, glm::vec3 color, float radius) 
    : Position(position), Color(color), Radius(radius) {
    setup();
}

void Sun::setup() {
    // Crear esfera simple con vértices
    vertices.clear();
    
    const int segments = 32;
    const int rings = 16;
    
    // Generar vértices de esfera
    for (int ring = 0; ring <= rings; ring++) {
        float phi = glm::pi<float>() * ring / rings;
        for (int segment = 0; segment <= segments; segment++) {
            float theta = 2.0f * glm::pi<float>() * segment / segments;
            
            glm::vec3 vertex(
                Radius * sin(phi) * cos(theta),
                Radius * cos(phi),
                Radius * sin(phi) * sin(theta)
            );
            
            vertices.push_back(vertex.x);
            vertices.push_back(vertex.y);
            vertices.push_back(vertex.z);
        }
    }
    
    // Crear buffers OpenGL
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Configurar atributos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glBindVertexArray(0);
}

void Sun::render(Shader& shader, glm::mat4 view, glm::mat4 projection) {
    shader.use();
    
    // Configurar uniforms del shader del sol
    shader.setVec3("sunPosition", Position);
    shader.setVec3("sunColor", Color);
    shader.setFloat("radius", Radius * 10.0f); // Radio de efecto glow
    shader.setVec3("viewPos", camera.Position);
    
    // Calcular matriz modelo
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, Position);
    model = glm::scale(model, glm::vec3(Radius));
    
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    
    // Renderizar esfera
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
    glBindVertexArray(0);
}

void Sun::updatePosition(glm::vec3 newPosition) {
    Position = newPosition;
}
