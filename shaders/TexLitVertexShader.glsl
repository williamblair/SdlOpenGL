#version 330 core

// attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// varyings to be sent to the fragment shader
out vec2 vTexCoord;
out vec3 vFragPos;
out vec3 vFragNorm;

// uniforms
uniform mat4 uMvpMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;

void main()
{
    gl_Position = uMvpMatrix * vec4( aPos, 1.0 );
    vTexCoord = aTexCoord;
    vFragPos = vec3( uModelMatrix * vec4( aPos, 1.0 ) );
    vFragNorm = normalize(
        vec3( uNormalMatrix * vec4( aNormal, 1.0 ) )
    );
}

