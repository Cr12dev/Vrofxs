#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Cube {
public:
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;

    Mesh* mesh;

    // Constructor con posición por defecto
    Cube(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f));

    // Transformaciones
    glm::mat4 GetModelMatrix() const;

    // Renderizado
    void Draw() const;
    void Draw(Shader& shader, Camera& camera) const;

    // Manipulación de posición
    void SetPosition(glm::vec3 pos);
    void Move(glm::vec3 offset);

private:
    void CreateCubeMesh();
};