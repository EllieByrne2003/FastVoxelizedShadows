#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 mvp;
uniform mat4 lightSpaceMatrix;

out vec3 FragPos;          // world space (optional, for lighting)
out vec3 Normal;           // world space
out vec2 TexCoords;
out vec4 FragPosLightSpace; // vertex position in light space

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    // Transform vertex to light clip space
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    gl_Position = mvp * vec4(aPos, 1.0);
}