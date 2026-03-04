#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 sunColor;      // Color principal del sol
uniform vec3 sunPosition;   // Posición del sol en mundo
uniform float radius;       // Radio del sol (para efecto glow)
uniform vec3 viewPos;       // Posición de la cámara

void main()
{
    // Vector desde el fragmento hacia el sol
    float distance = length(FragPos - sunPosition);
    
    // Factor de atenuación radial (brillo central más fuerte)
    float intensity = 1.0 - clamp(distance / radius, 0.0, 1.0);
    intensity = pow(intensity, 4.0); // suavizado con potencia más fuerte
    
    // Efecto de borde suave (anti-aliasing)
    float edgeFactor = 1.0 - smoothstep(0.8 * radius, 1.2 * radius, distance);
    
    // Color base con brillo
    vec3 color = sunColor * intensity;
    
    // Añadir ligero efecto de fresnel en el borde
    float fresnel = pow(1.0 - dot(normalize(FragPos - viewPos), normalize(Normal)), 2.0);
    color = mix(color * 0.8, color, fresnel);
    
    // Color final con todo los efectos
    FragColor = vec4(color * edgeFactor, 1.0);
}