#include "include/Sun.h"
#include <glm/gtc/constants.hpp>

Sun::Sun(glm::vec3 position, glm::vec3 color, float radius)
    : Position(position), Color(color), Radius(radius)
{
    setup();
}

Sun::~Sun()
{
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void Sun::setup()
{
    generateSphere(48, 24);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glBindVertexArray(0);
}

void Sun::generateSphere(int segments, int rings)
{
    vertices.clear();
    indices.clear();

    for (int ring = 0; ring <= rings; ring++)
    {
        float v = (float)ring / rings;
        float phi = v * glm::pi<float>();

        for (int segment = 0; segment <= segments; segment++)
        {
            float u = (float)segment / segments;
            float theta = u * glm::two_pi<float>();

            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for (int ring = 0; ring < rings; ring++)
    {
        for (int segment = 0; segment < segments; segment++)
        {
            int current = ring * (segments + 1) + segment;
            int next = current + segments + 1;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }
}

void Sun::render(Shader& shader,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& viewPos)
{
    shader.use();

    shader.setVec3("sunPosition", Position);
    shader.setVec3("sunColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setFloat("radius", Radius * 15.0f);
    shader.setVec3("viewPos", viewPos);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, Position);
    model = glm::scale(model, glm::vec3(Radius));

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Sun::updatePosition(glm::vec3 newPosition)
{
    Position = newPosition;
}