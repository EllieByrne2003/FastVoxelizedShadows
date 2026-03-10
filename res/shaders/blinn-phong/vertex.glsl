#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 mvp;
uniform mat4 model;

out vec3 pos;
out vec3 normal;
out vec2 texCoords;
out vec4 posLightSpace;

uniform mat4 lightSpaceMatrix;

void main() {
    pos       = vec3(model * vec4(aPos, 1.0));
    normal    = mat3(transpose(inverse(model))) * aNormal;
    texCoords = aTexCoords;


    posLightSpace = lightSpaceMatrix * vec4(pos, 1.0);

    gl_Position = mvp * vec4(aPos, 1.0);
}