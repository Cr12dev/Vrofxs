#include "Cube.h"

Cube::Cube(glm::vec3 pos) {
    position = pos;
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    
    CreateCubeMesh();
}

void Cube::CreateCubeMesh() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float size = 0.5f;
    
    vertices = {
        // Front face
        {{-size, -size,  size}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{ size, -size,  size}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{ size,  size,  size}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-size,  size,  size}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        
        // Back face
        {{-size, -size, -size}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-size,  size, -size}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ size,  size, -size}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
        {{ size, -size, -size}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
        
        // Top face
        {{-size,  size, -size}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{-size,  size,  size}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ size,  size,  size}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ size,  size, -size}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        
        // Bottom face
        {{-size, -size, -size}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{ size, -size, -size}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
        {{ size, -size,  size}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{-size, -size,  size}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        
        // Right face
        {{ size, -size, -size}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ size,  size, -size}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{ size,  size,  size}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{ size, -size,  size}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        
        // Left face
        {{-size, -size, -size}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-size, -size,  size}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-size,  size,  size}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-size,  size, -size}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}
    };
    
    indices = {
        0,  1,  2,  0,  2,  3,   // Front
        4,  5,  6,  4,  6,  7,   // Back
        8,  9,  10, 8,  10, 11,  // Top
        12, 13, 14, 12, 14, 15,  // Bottom
        16, 17, 18, 16, 18, 19,  // Right
        20, 21, 22, 20, 22, 23   // Left
    };
    
    mesh = new Mesh(vertices, indices);
}

glm::mat4 Cube::GetModelMatrix() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}

void Cube::Draw() {
    mesh->Draw();
}

void Cube::SetPosition(glm::vec3 pos) {
    position = pos;
}

void Cube::Move(glm::vec3 offset) {
    position += offset;
}
