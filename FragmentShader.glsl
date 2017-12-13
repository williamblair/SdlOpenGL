#version 330 core

out vec4 FragColor;

in vec4 fragColor;
in vec2 texCoord;

uniform sampler2D ourTexture;

void main()
{
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    //FragColor = fragColor;
    //FragColor = texture(ourTexture, texCoord);
    FragColor = texture(ourTexture, texCoord) * fragColor;
}
