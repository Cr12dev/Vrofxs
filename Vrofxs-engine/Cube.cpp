#include "include/Cube.h"
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

    vertices = {
        // Front
        {{-size, -size,  size}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{ size, -size,  size}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{ size,  size,  size}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-size,  size,  size}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

        // Back
        {{-size, -size, -size}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-size,  size, -size}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ size,  size, -size}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
        {{ size, -size, -size}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},

        // Top
        {{-size,  size, -size}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{-size,  size,  size}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ size,  size,  size}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ size,  size, -size}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},

        // Bottom
        {{-size, -size, -size}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{ size, -size, -size}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
        {{ size, -size,  size}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{-size, -size,  size}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},

        // Right
        {{ size, -size, -size}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ size,  size, -size}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{ size,  size,  size}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{ size, -size,  size}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

        // Left
        {{-size, -size, -size}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-size, -size,  size}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-size,  size,  size}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-size,  size, -size}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}
    };

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
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}

void Cube::Draw() const {
    if (mesh)
        mesh->Draw();
}

void Cube::Draw(Shader& shader, Camera& camera) const {
    if (!mesh) return;

    shader.use();

    // Material y luz
    shader.setVec3("objectColor", glm::vec3(0.8f, 0.8f, 0.8f));
    shader.setFloat("shininess", 32.0f);
    shader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
    shader.setVec3("lightColor", glm::vec3(1.0f, 0.95f, 0.8f));
    shader.setVec3("lightDir", glm::vec3(0.0f, -1.0f, 0.0f));
    shader.setFloat("ambient", 0.2f);
    shader.setFloat("diffuse", 1.0f);
    shader.setFloat("specular", 0.5f);

    // Matrices
    shader.setMat4("model", GetModelMatrix());
    shader.setMat4("view", camera.GetViewMatrix());

    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), float(width) / float(height), 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    mesh->Draw();
}

void Cube::SetPosition(glm::vec3 pos) {
    position = pos;
}

void Cube::Move(glm::vec3 offset) {
    position += offset;
}