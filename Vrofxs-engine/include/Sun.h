#pragma once
#include "Main.h"
#include <vector>

class Sun {
public:
    glm::vec3 Position;
    glm::vec3 Color;
    float Radius;
    unsigned int VAO, VBO;
    std::vector<float> vertices;
    
    Sun(glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f), 
         glm::vec3 color = glm::vec3(1.0f, 0.9f, 0.0f), 
         float radius = 0.5f);
    
    void setup();
    void render(Shader& shader, glm::mat4 view, glm::mat4 projection);
    void updatePosition(glm::vec3 newPosition);
};
