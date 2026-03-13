#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Light {
private:
    // Alignment is important for SSBO
    const vec3 position  alignas(sizeof(vec4));
    const vec3 direction alignas(sizeof(vec4));
    const vec3 colour    alignas(sizeof(vec4));

    const float intensity;
    
    int depthMapIndex = -1; // None assigned 

    // TODO add some data to let lights be directional or spot lights

protected:

public:
    Light(const vec3 &position, const vec3 &direction, const vec3 &colour, const float intensity);
    ~Light();

    static Light * readLight(const json &jsonLight);

    mat4 getView() const;

    void setDepthMapIndex(const int depthMapIndex);
};