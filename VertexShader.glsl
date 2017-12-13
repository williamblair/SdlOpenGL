#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

// send the given color to the fragment shader
out vec4 fragColor;

void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   fragColor   = vec4(aColor, 1.0);
}
