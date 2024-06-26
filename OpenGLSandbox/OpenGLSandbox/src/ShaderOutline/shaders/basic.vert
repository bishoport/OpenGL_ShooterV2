#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
	vec3 pos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f));
	gl_Position = camMatrix * vec4(pos, 1.0);
}