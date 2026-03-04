#pragma once
#include <vector>
#include <GL/glew.h>
#include "Main.h"

extern unsigned int gridVAO, gridVBO;
extern std::vector<float> gridVertices;
void setupGrid(int size, float spacing);
void renderGrid(int size, float spacing);