#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Cube {
public:
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    
    Mesh* mesh;
    
    Cube(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f));
    
    glm::mat4 GetModelMatrix();
    void Draw();
    
    void SetPosition(glm::vec3 pos);
    void Move(glm::vec3 offset);

private:
    void CreateCubeMesh();
};
