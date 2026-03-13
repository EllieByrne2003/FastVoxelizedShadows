#version 430 core

const int MAX_LIGHTS = 256;

in vec4 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Light {
    mat4  lightSpaceMatrix;   // 64 bytes
    vec3  position;           // 16 bytes (vec4 in std430)
    vec3  direction;          // 16 bytes
    vec3  colour;             // 16 bytes
    float intensity;          //  4 bytes
    float coneAngle;          //  4 bytes
    int   depthMapIndex;      //  4 bytes
    int   voxelRootIndex;     //  4 bytes
};

layout(std430, binding = 0) buffer LightBlock {
    Light lights[];
};                   // implicit padding to 128 bytes per element
uniform int lightCount;

uniform sampler2D      textureMap;
uniform sampler2DArray depthMaps;

uniform vec3 cameraPos;

out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace, float bias, int index)
{
    vec3 coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    coords = coords * 0.5 + 0.5;

    if (coords.x < 0.0 || coords.x > 1.0 || coords.y < 0.0 || coords.y > 1.0 || coords.z < 0.0 || coords.z > 1.0) {
        return 1.0; // No shadow contribution (fully lit)
    }

    vec4 texPosition = vec4(coords.xy, float(index), coords.z - bias);
    
    float currentDepth = coords.z - bias;

    float shadow    = 0.0;
    vec2  texelSize = 1.0 / textureSize(depthMaps, 0).xy;
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            vec3  pcfPosition = vec3(texPosition.xy + vec2(x, y) * texelSize, index);
            float pcfDepth    = texture(depthMaps, pcfPosition).r;
            shadow += (pcfDepth == 1.0 || currentDepth > pcfDepth) ? 1.0 : 0.0;
            // shadow += (currentDepth > pcfDepth) ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;
}

void main()
{
    vec4 texColour    = texture(textureMap, TexCoords);
    vec3 diffuseColor = texColour.xyz;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPos - FragPos.xyz);

    vec3 result = 0.1 * diffuseColor;

    for(int i = 0; i < lightCount; i++) {
        Light light = lights[i];
        vec3 lightDir = normalize(light.direction);

        // Spotlight direction correction
        if(light.coneAngle != -1.0f) {
            lightDir = normalize(FragPos.xyz - light.position);
        
            float cosAngle     = dot(lightDir, normalize(light.direction));
            float cosConeAngle = cos(radians(light.coneAngle * 0.5));

            if(cosAngle < cosConeAngle) {
                continue;
            }
        }

        vec4  fragPosLightSpace = light.lightSpaceMatrix * FragPos;
        float bias              = max(0.0005 * (1.0 - dot(norm, -lightDir)), 0.0005);
        float shadow            = ShadowCalculation(fragPosLightSpace, bias, light.depthMapIndex);

        float diff    = max(dot(norm, -lightDir), 0.0);
        vec3  diffuse = diff * light.colour * diffuseColor;

        // Blinn phong
        vec3  halfwayDir = normalize(viewDir - lightDir);
        float spec       = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
        vec3  specular   = spec * light.colour;

        result += (1.0 - shadow) * (diffuse + specular);
    }

    FragColor = vec4(result, texColour.z);
}