#version 460 core

uniform sampler2D diffuseTexture;

in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); // Color naranja
    FragColor = texture(diffuseTexture, TexCoord);
}
