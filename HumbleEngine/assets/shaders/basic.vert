#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 camMatrix; //view + projection

out vec2 TexCoord;

void main()
{
    gl_Position = camMatrix * model * vec4(aPos, 1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);

    //gl_Position = vec4(aPos, 1.0);
}
