#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

class Sun {
public:

    glm::vec3 Position;
    glm::vec3 Color;
    float Radius;

    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    Sun(glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f),
        glm::vec3 color = glm::vec3(1.0f, 0.9f, 0.3f),
        float radius = 1.0f);

    ~Sun();

    void render(Shader& shader,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& viewPos);

    void updatePosition(glm::vec3 newPosition);

private:

    void setup();
    void generateSphere(int segments, int rings);
};