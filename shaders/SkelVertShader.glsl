#version 330 core

layout (location = 0) in vec4 aPos;

// the transformation matrix
uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos.xyz, 1.0);
}

