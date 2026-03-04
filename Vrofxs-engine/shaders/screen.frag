#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float time;

// toggles
uniform bool invert;
uniform bool grayscale;
uniform bool edge;

void main()
{
    vec2 uv = TexCoords;

    vec3 col = texture(screenTexture, uv).rgb;

    // chromatic aberration
    float offset = 0.003;
    float r = texture(screenTexture, uv + vec2(offset,0)).r;
    float g = texture(screenTexture, uv).g;
    float b = texture(screenTexture, uv - vec2(offset,0)).b;
    col = vec3(r,g,b);

    // grayscale
    if(grayscale)
    {
        float avg = (col.r + col.g + col.b) / 3.0;
        col = vec3(avg);
    }

    // invert
    if(invert)
    {
        col = vec3(1.0) - col;
    }

    // edge detection
    if(edge)
    {
        float kernel[9] = float[](
            -1,-1,-1,
            -1, 8,-1,
            -1,-1,-1
        );

        vec2 offsets[9] = vec2[](
            vec2(-0.001,  0.001), // top-left
            vec2( 0.0f,    0.001),
            vec2( 0.001,  0.001),
            vec2(-0.001,  0.0),
            vec2( 0.0,     0.0),
            vec2( 0.001,  0.0),
            vec2(-0.001, -0.001),
            vec2( 0.0,    -0.001),
            vec2( 0.001, -0.001)
        );

        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture(screenTexture, uv + offsets[i]));
        }

        vec3 edgeColor = vec3(0.0);
        for(int i = 0; i < 9; i++)
        {
            edgeColor += sampleTex[i] * kernel[i];
        }

        col = edgeColor;
    }

    FragColor = vec4(col, 1.0);
}