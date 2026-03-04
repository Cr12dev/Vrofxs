uniform vec3 lineColor; // color que vas a setear desde C++
out vec4 FragColor;
void main() {
    FragColor = vec4(lineColor, 1.0);
}