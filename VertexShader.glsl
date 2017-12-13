#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

// send the given color to the fragment shader
out vec4 fragColor;

// send the texture location to the fragment shader
out vec2 texCoord;

void main()
{
   gl_Position = vec4(aPos, 1.0);
   fragColor   = vec4(aColor, 1.0);
   texCoord = aTexCoord;
}
