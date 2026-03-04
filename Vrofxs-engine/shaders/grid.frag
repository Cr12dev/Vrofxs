#version 330 core
uniform vec3 lineColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(lineColor, 0.8); // Slightly transparent for better visibility
}