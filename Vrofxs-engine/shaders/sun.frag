#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 sunColor;

void main()
{
    // Distancia desde el centro usando la normal
    float dist = length(Normal);

    // Núcleo brillante
    float core = 1.0 - dist;
    core = clamp(core, 0.0, 1.0);
    core = pow(core, 2.0);

    // Halo exterior
    float glow = exp(-dist * 4.0);

    float intensity = core + glow * 0.5;

    vec3 color = sunColor * intensity;

    FragColor = vec4(color, 1.0);
}