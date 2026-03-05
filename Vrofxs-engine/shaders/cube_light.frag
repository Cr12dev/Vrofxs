#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

// Material properties
uniform vec3 objectColor;
uniform float shininess;

// Light properties
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform float ambient;
uniform float diffuse;
uniform float specular;

// Camera position
uniform vec3 viewPos;

// Shadow map
uniform sampler2D shadowMap;
uniform float shadowBias;
uniform int pcfSamples;

// PCF function for soft shadows
float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Early exit if outside shadow map
    if(projCoords.z > 1.0)
        return 0.0;
    
    // Get current depth
    float currentDepth = projCoords.z;
    
    // Bias to prevent shadow acne
    float bias = max(shadowBias * (1.0 - dot(normal, lightDir)), shadowBias * 0.05);
    
    // PCF - Percentage Closer Filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    // Kernel size based on pcfSamples (1 = 3x3, 2 = 5x5, etc.)
    int halfSamples = max(pcfSamples / 2, 1);
    
    for(int x = -halfSamples; x <= halfSamples; ++x)
    {
        for(int y = -halfSamples; y <= halfSamples; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float pcfDepth = texture(shadowMap, projCoords.xy + offset).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    
    // Average the samples
    float totalSamples = float((halfSamples * 2 + 1) * (halfSamples * 2 + 1));
    shadow /= totalSamples;
    
    return shadow;
}

void main()
{
    // Normalize inputs
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDirection = normalize(viewPos - FragPos);
    
    // Ambient lighting (always applied, not shadowed)
    vec3 ambientColor = ambient * lightColor * objectColor;
    
    // Calculate shadow with PCF
    float shadow = calculateShadow(FragPosLightSpace, norm, lightDirection);
    
    // Diffuse lighting (affected by shadow)
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuseColor = (1.0 - shadow) * diff * lightColor * objectColor;
    
    // Specular lighting (affected by shadow)
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);
    vec3 specularColor = (1.0 - shadow) * spec * lightColor;
    
    // Combine all lighting components
    vec3 result = ambientColor + diffuseColor + specularColor;
    
    FragColor = vec4(result, 1.0);
}
