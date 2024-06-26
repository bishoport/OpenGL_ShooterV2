#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 u_Color; // Uniforme para el color
uniform sampler2D texture1;
uniform bool use_texture;


void main() 
{
    if(use_texture)
    {
         FragColor = texture(texture1, TexCoord);
    }
    else
    {
        FragColor = u_Color;
    }
   
}