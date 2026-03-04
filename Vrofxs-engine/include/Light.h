#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Light {
public:
    glm::vec3 Position;
    glm::vec3 Color;
    float Intensity;
    float Ambient;
    float Constant;
    float Linear;
    float Quadratic;
    
    Light(glm::vec3 position = glm::vec3(0.0f, 10.0f, 0.0f), 
          glm::vec3 color = glm::vec3(1.0f, 0.95f, 0.8f), 
          float intensity = 1.0f)
        : Position(position), Color(color), Intensity(intensity), Ambient(0.2f), 
          Constant(1.0f), Linear(0.09f), Quadratic(0.032f) {
        // Reference to sun for directional light calculation
        sunPosition = position;
        sunColor = color;
        sunIntensity = intensity;
    }
    
    void updatePosition(glm::vec3 newPosition) {
        Position = newPosition;
    }
    
    // Static sun variables (accessible from other classes)
    static glm::vec3 sunPosition;
    static glm::vec3 sunColor;
    static float sunIntensity;
};
