#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 aTexCoords;

out vec4 FragColor;

// Material properties
uniform vec3 objectColor;
uniform float shininess;

// Light properties (using static sun variables)
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform float ambient;
uniform float diffuse;
uniform float specular;

// Camera position for specular calculation
uniform vec3 viewPos;

void main()
{
    // Ambient lighting
    vec3 ambientColor = ambient * lightColor * objectColor;
    
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuseColor = diff * lightColor * objectColor;
    
    // Specular lighting
    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);
    vec3 specularColor = spec * lightColor;
    
    // Combine all lighting components
    vec3 result = ambientColor + diffuseColor + specularColor;
    
    FragColor = vec4(result, 1.0);
}
