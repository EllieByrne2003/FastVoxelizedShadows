#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Light {
private:
    // Alignment is important for SSBO
    mat4 lightSpaceMatrix alignas(sizeof(vec4));

    const vec3 position  alignas(sizeof(vec4));
    const vec3 direction alignas(sizeof(vec4));
    const vec3 colour    alignas(sizeof(vec4));

    const float intensity;
    const float coneAngle;  // Directional if -1.0f
    
    int depthMapIndex  = -1; // None assigned 
    int voxelRootIndex = -1; // None assigned

    // TODO add some data to let lights be directional or spot lights

protected:

public:
    Light(const vec3 &position, const vec3 &direction, const vec3 &colour, const float intensity, const float coneAngle = -1.0f);
    ~Light();

    static Light * readLight(const json &jsonLight);

    mat4 getView() const;

    bool isDirectional() const;
    bool isSpotty() const;

    void setDepthMapIndex(const int depthMapIndex);
    void setVoxelRootIndex(const int voxelRootIndex);

    void setLightSpaceMatrix(const mat4 &proj);
    mat4 getLightSpaceMatrix() const;

    vec3 getPosition() const;
    vec3 getDirection() const;

    float getIntensity() const;
    float getConeAngle() const;
};