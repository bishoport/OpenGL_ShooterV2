#version 460 core



uniform sampler2D diffuseTexture;

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos; 

out vec4 FragColor;


void main()
{
    FragColor = texture(diffuseTexture, TexCoord);
}
