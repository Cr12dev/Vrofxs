#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Axis {
public:
    unsigned int VAO, VBO;
    
    Axis();
    
    void Draw(glm::mat4 view, glm::mat4 projection);

private:
    void setupAxis();
};
