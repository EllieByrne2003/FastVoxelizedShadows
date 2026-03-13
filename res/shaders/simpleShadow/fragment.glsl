#version 430 core

const int MAX_LIGHTS = 256;

in vec4 FragPos;
in vec3 Normal;
in vec2 TexCoords;
// in vec4 FragPosLightSpace;

struct Light {
    mat4  lightSpaceMatrix;   // 64 bytes
    vec3  position;           // 16 bytes (vec4 in std430)
    vec3  direction;          // 16 bytes
    vec3  colour;             // 16 bytes
    float intensity;          //  4 bytes
    int   depthMapIndex;      //  4 bytes
};

layout(std430, binding = 0) buffer LightBlock {
    Light lights[];
};                   // implicit padding to 128 bytes per element
uniform int lightCount;

uniform sampler2D            textureMap;
// uniform sampler2DArrayShadow depthMaps;   // shadow sampler!
uniform sampler2DArray depthMaps;   // shadow sampler!
// uniform sampler2D shadowMap;   // shadow sampler!

uniform vec3 cameraPos;

out vec4 FragColor;

// const vec3 lightPos = vec3(0, 100, 100);
// ... other uniforms for material, etc.

float ShadowCalculation(vec4 fragPosLightSpace, float bias, int index)
{
    vec3 coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    coords = coords * 0.5 + 0.5;
    // vec4 texPosition = vec4(fragPosLightSpace.xy, float(index), fragPosLightSpace.z);
    vec4 texPosition = vec4(coords.xy, float(index), coords.z - bias);
    // vec3 texPosition = vec4(coords.xy, float(index));
    
    // // Perform perspective divide to get normalized device coordinates
    // vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // // Transform to [0,1] range (standard for texture lookup)
    // projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth from light's perspective (using textureProj)
    // float closestDepth = textureProj(shadowMap, fragPosLightSpace);
    // fragPosLightSpace.xyw = fragPosLightSpace.xyz;
    // fragPosLightSpace.z = index;
    // float closestDepth = textureProj(depthMaps, fragPosLightSpace);

    // Alternatively, with a standard sampler2D you would do:
    // float closestDepth = (texture(depthMaps, vec3(projCoords.xy, index)).r - 0.5) * 2.0;
    // float closestDepth = texture(depthMaps, texPosition).r;
    
    float currentDepth = (fragPosLightSpace.z - bias) / 2.0 + 0.5;

    float shadow    = 0.0;
    vec2  texelSize = 1.0 / textureSize(depthMaps, 0).xy;
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            vec3 pcfPosition = vec3(texPosition.xy + vec2(x, y) * texelSize, index);
            shadow += currentDepth > texture(depthMaps, pcfPosition).r ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;

    // return closestDepth;  // 1.0 if lit, 0.0 if shadowed (or fractional for PCF)
}

void main()
{
    // ... compute lighting (ambient, diffuse, specular) ...

    // Light light = lights[0];

    // vec4 FragPosLightSpace = light.lightSpaceMatrix * FragPos;
    // // vec4 FragPosLightSpace = lightSpaceMatrix * FragPos;

    // // float closestDepth = ShadowCalculation(FragPosLightSpace, light.depthMapIndex);
    // // float bias = max(0.02 * (1.0 - dot(Normal, light.direction)), 0.002); 
    // // // float shadow = closestDepth - bias > FragPosLightSpace.z ? 1.0 : 0.0; 
    // // // Combine: final color = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor
    
    // float bias = max(0.03 * (1.0 - dot(normalize(Normal), normalize(light.direction))), 0.005);
    // float closestDepth = ShadowCalculation(FragPosLightSpace, bias, light.depthMapIndex);

    vec4 texColour    = texture(textureMap, TexCoords);
    vec3 diffuseColor = texColour.xyz;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPos - FragPos.xyz);

    vec3 result = 0.1 * diffuseColor;

    for(int i = 0; i < lightCount; i++) {
        Light light = lights[i];
        vec3 lightDir = normalize(light.direction);

        vec4  fragPosLightSpace = light.lightSpaceMatrix * FragPos;
        float bias              = max(0.03 * (1.0 - dot(norm, lightDir)), 0.005);
        float shadow            = ShadowCalculation(fragPosLightSpace, bias, light.depthMapIndex);

        float diff    = max(dot(norm, -lightDir), 0.0);
        vec3  diffuse = diff * light.colour * diffuseColor;

        // Blinn phong
        vec3  halfwayDir = normalize(viewDir - lightDir);
        float spec       = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
        vec3  specular   = spec * light.colour;

        result += (1.0 - shadow) * (diffuse + specular);
    }


    // // // vec3 finalColor = light.colour * shadowValue;
    // vec3 finalColor = vec3(1.0 - closestDepth);
                    
    // // vec3 finalColor;
    // // if(FragPosLightSpace.z - bias > closestDepth) {
    // //     finalColor = vec3(0.0, 0.0, 1.0);
    // // } else {
    // //     finalColor = light.colour;
    // // }

    // // FragColor = vec4(finalColor, 1.0);
    // FragColor = vec4(texture(textureMap, TexCoords));

    FragColor = vec4(result, texColour.z);
}