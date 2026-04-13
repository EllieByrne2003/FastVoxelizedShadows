#version 330

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2  TexCoords;

void main() {
    TexCoords = aTexCoords;

    gl_Position = proj * view * model * vec4(aPos, 1.0);
}