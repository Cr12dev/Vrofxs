#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;     // Posición de la luz en mundo
uniform vec3 viewPos;      // Posición de la cámara
uniform vec3 objectColor;  // Color del cubo
uniform vec3 lightColor;   // Color de la luz

void main()
{
    // --- Ambient ---
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // --- Diffuse ---
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // --- Specular ---
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // --- Combinación final ---
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}