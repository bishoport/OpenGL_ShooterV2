#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 u_Color; // Uniforme para el color

void main() 
{
     FragColor = u_Color;
   
}