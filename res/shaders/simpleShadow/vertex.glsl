#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform mat4 lightSpaceMatrix;

// out vec3 FragPos;          // world space (optional, for lighting)
out vec3 Normal;           // world space
// out vec2 TexCoords;
out vec4 FragPosLightSpace; // vertex position in light space

void main()
{
    // FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    // TexCoords = aTexCoords;

    // Transform vertex to light space
    FragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.0);

    gl_Position = proj * view * model * vec4(aPos, 1.0);
}