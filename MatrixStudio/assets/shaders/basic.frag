#version 460 core
out vec4 FragColor;

uniform vec3 inputColor;

void main()
{
FragColor = vec4(vec3(inputColor), 1.0f);
}