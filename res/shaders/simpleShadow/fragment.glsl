#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform sampler2D texture;
uniform sampler2DShadow shadowMap;   // shadow sampler!

uniform vec3 cameraPos;

const vec3 lightPos = vec3(0, 100, 100);
// ... other uniforms for material, etc.

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Perform perspective divide to get normalized device coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range (standard for texture lookup)
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth from light's perspective (using textureProj)
    float closestDepth = textureProj(shadowMap, fragPosLightSpace);

    // Alternatively, with a standard sampler2D you would do:
    // float closestDepth = texture(shadowMap, projCoords.xy).r;
    // But with sampler2DShadow and textureProj, the result is the shadow factor directly.

    return closestDepth;  // 1.0 if lit, 0.0 if shadowed (or fractional for PCF)
}

void main()
{
    // ... compute lighting (ambient, diffuse, specular) ...

    float shadow = ShadowCalculation(FragPosLightSpace);

    // Combine: final color = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor
    
    vec3 finalColor;
    if(shadow >= 1.0) {
        finalColor = vec3(1.0, 0.0, 0.0);
    } else {
        finalColor = vec3(0.0, 0.0, 0.0);
    }

    finalColor = vec3(0.0, 1.0, 0.0);

    FragColor = vec4(finalColor, 1.0);
}