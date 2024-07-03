#version 460 core

out vec4 FragColor;

in vec3 WorldPos;
in vec2 TexCoord;
in vec3 Normal;  

uniform sampler2D albedoTexture;

void main()
{    
    FragColor = texture(albedoTexture, TexCoord);
}


