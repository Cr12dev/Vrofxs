#include "GridCalc.h"

std::vector<float> gridVertices;

void setupGrid(int size, float spacing)
{
    gridVertices.clear();

    for (int i = -size; i <= size; i++)
    {
        // Líneas paralelas a Z (eje X)
        gridVertices.push_back(i * spacing); gridVertices.push_back(0); gridVertices.push_back(-size * spacing);
        gridVertices.push_back(i * spacing); gridVertices.push_back(0); gridVertices.push_back(size * spacing);

        // Líneas paralelas a X (eje Z)
        gridVertices.push_back(-size * spacing); gridVertices.push_back(0); gridVertices.push_back(i * spacing);
        gridVertices.push_back(size * spacing);  gridVertices.push_back(0); gridVertices.push_back(i * spacing);
    }

    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}