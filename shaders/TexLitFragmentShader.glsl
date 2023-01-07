#version 330 core

// final fragment color
out vec4 FragColor;

// uniforms
uniform sampler2D uTexture0;
uniform vec3 uAmbient;
uniform vec3 uPosLgtPos0; // position
uniform vec3 uPosLgtDff0; // diffuse
uniform vec3 uDirLgtDir0;
uniform vec3 uDirLgtDff0;

// varyings from the vertex shader
in vec2 vTexCoord;
in vec3 vFragPos;
in vec3 vFragNorm;

// calculate positional lighting contribution
vec3 calcPosLgt()
{
    vec3 norm = normalize( vFragNorm );
    vec3 lightDir = normalize( uPosLgtPos0 - vFragPos );
    float nDotL = max( dot( lightDir, norm ), 0.0 );
    vec3 diffuse =
        uPosLgtDff0 *
        texture( uTexture0, vTexCoord ).rgb *
        nDotL;
    return diffuse;
}

// calculate directional lighting contribution
vec3 calcDirLgt()
{
    vec3 norm = normalize( vFragNorm );
    vec3 lightDir = normalize( -uDirLgtDir0 );
    float nDotL = max( dot( lightDir, norm ), 0.0 );
    vec3 diffuse = 
        uDirLgtDff0 *
        texture( uTexture0, vTexCoord ).rgb *
        nDotL;
    return diffuse;
}

// calculate ambient lighting contribution
vec3 calcAmbLgt()
{
    return uAmbient * texture( uTexture0, vTexCoord ).rgb;
}

void main()
{
    FragColor = vec4(
        calcPosLgt() + calcDirLgt() + calcAmbLgt(),
        1.0
    );
}

