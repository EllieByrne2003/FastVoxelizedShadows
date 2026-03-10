#version 330 core

in vec3 pos;
in vec3 normal;
in vec2 texCoords;
in vec4 posLightSpace;

uniform vec3  cameraPos;
uniform float shininess;

// Lighting constants
const vec3  LIGHT_POS       = vec3(0.0, 100, 100);
const vec3  LIGHT_DIR       = normalize(LIGHT_POS);
const vec3  LIGHT_COLOR     = vec3(1.0, 1.0, 1.0);
const float LIGHT_INTENSITY = 1.0;

// Shading constants
const vec3  AMBIENT_COLOR  = vec3(0.1, 0.1, 0.1);
const vec3  DIFFUSE_COLOR  = vec3(0.5, 0.5, 0.5);
const vec3  SPECULAR_COLOR = vec3(0.0, 1.0, 0.0);

uniform sampler2D textureMap;
uniform sampler2D shadowMap;

out vec4 color;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}  

void main() {
    vec3 ambient = AMBIENT_COLOR * LIGHT_COLOR;

    float diff     = max(dot(normalize(normal), LIGHT_DIR), 0.0);
    vec3  diffuse  = diff * DIFFUSE_COLOR * LIGHT_COLOR * LIGHT_INTENSITY;

    vec3  viewDir    = normalize(cameraPos - pos);
    vec3  halfwayDir = normalize(LIGHT_DIR + viewDir);
    float spec       = pow(max(dot(normalize(normal), halfwayDir), 0.0), shininess);
    vec3  specular   = spec * SPECULAR_COLOR * LIGHT_COLOR * LIGHT_INTENSITY;

    vec3 result = ambient + (1 - ShadowCalculation(posLightSpace)) * (diffuse + specular);

    color = vec4(result, 1.0);
}