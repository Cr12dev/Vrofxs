#include "include/Cube.h"
#include "include/Shader.h"
#include "include/Camera.h"
#include <GLFW/glfw3.h>
#include <vector>

Cube::Cube(glm::vec3 pos)
    : position(pos), scale(1.0f), rotation(0.0f), mesh(nullptr)
{
    CreateCubeMesh();
}

void Cube::CreateCubeMesh() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float size = 0.5f;

    // Vértices con normales
    vertices = {
        // Front
        {{-size,-size, size}, {0,0,1}, {0,0}},
        {{ size,-size, size}, {0,0,1}, {1,0}},
        {{ size, size, size}, {0,0,1}, {1,1}},
        {{-size, size, size}, {0,0,1}, {0,1}},

        // Back
        {{-size,-size,-size}, {0,0,-1}, {1,0}},
        {{-size, size,-size}, {0,0,-1}, {1,1}},
        {{ size, size,-size}, {0,0,-1}, {0,1}},
        {{ size,-size,-size}, {0,0,-1}, {0,0}},

        // Top
        {{-size, size,-size}, {0,1,0}, {0,1}},
        {{-size, size, size}, {0,1,0}, {0,0}},
        {{ size, size, size}, {0,1,0}, {1,0}},
        {{ size, size,-size}, {0,1,0}, {1,1}},

        // Bottom
        {{-size,-size,-size}, {0,-1,0}, {1,1}},
        {{ size,-size,-size}, {0,-1,0}, {0,1}},
        {{ size,-size, size}, {0,-1,0}, {0,0}},
        {{-size,-size, size}, {0,-1,0}, {1,0}},

        // Right
        {{ size,-size,-size}, {1,0,0}, {1,0}},
        {{ size, size,-size}, {1,0,0}, {1,1}},
        {{ size, size, size}, {1,0,0}, {0,1}},
        {{ size,-size, size}, {1,0,0}, {0,0}},

        // Left
        {{-size,-size,-size}, {-1,0,0}, {0,0}},
        {{-size,-size, size}, {-1,0,0}, {1,0}},
        {{-size, size, size}, {-1,0,0}, {1,1}},
        {{-size, size,-size}, {-1,0,0}, {0,1}}
    };

    // Índices
    indices = {
        0,1,2, 0,2,3,      // Front
        4,5,6, 4,6,7,      // Back
        8,9,10, 8,10,11,   // Top
        12,13,14, 12,14,15,// Bottom
        16,17,18, 16,18,19,// Right
        20,21,22, 20,22,23 // Left
    };

    mesh = new Mesh(vertices, indices);
}

glm::mat4 Cube::GetModelMatrix() const {
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);
    return model;
}

void Cube::Draw(Shader& shader, Camera& camera) const {
    if (!mesh) return;

    shader.use();

    // Configuración de luz y material
    shader.setVec3("lightPos", glm::vec3(2, 2, 2));
    shader.setVec3("lightColor", glm::vec3(1, 1, 1));
    shader.setVec3("viewPos", camera.Position);

    shader.setMat4("model", GetModelMatrix());
    shader.setMat4("view", camera.GetViewMatrix());

    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), float(width) / float(height), 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    mesh->Draw();
}

void Cube::SetPosition(glm::vec3 pos) { position = pos; }
void Cube::Move(glm::vec3 offset) { position += offset; }