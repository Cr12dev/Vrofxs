#version 330 core

in vec3 FragPos;

out vec4 FragColor;

uniform vec3 sunColor;
uniform vec3 viewPos;

void main()
{
    // vector cámara → fragmento
    vec3 viewDir = normalize(viewPos - FragPos);

    // efecto fresnel para borde brillante
    float fresnel = pow(1.0 - max(dot(viewDir, vec3(0.0,0.0,1.0)), 0.0), 3.0);

    // brillo base del sol
    float intensity = 1.2;

    // glow suave
    float glow = fresnel * 0.8;

    vec3 color = sunColor * (intensity + glow);

    FragColor = vec4(color, 1.0);
}