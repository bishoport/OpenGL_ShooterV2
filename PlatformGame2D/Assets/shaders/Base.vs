#version 460 core
layout(location = 0) in vec3 aPos;   // Posición del vértice en el espacio 2D
layout(location = 1) in vec2 aTexCoord; // Coordenadas de textura

out vec2 TexCoord; // Para pasar a fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    TexCoord = aTexCoord;
}
