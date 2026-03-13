#include "light.hpp"

#include <glm/ext/matrix_transform.hpp>

Light::Light(const vec3 &position, const vec3 &direction, const vec3 &colour, const float intensity) :
    position(position), direction(direction), colour(colour), intensity(intensity)
{

}

Light::~Light() {

}

Light * Light::readLight(const json &jsonLight) {
    if(!jsonLight.contains("position")) {
        return nullptr;
    }

    if(!jsonLight.contains("direction")) {
        return nullptr;
    }

    if(!jsonLight.contains("colour")) {
        return nullptr;
    }
    
    if(!jsonLight.contains("intensity")) {
        return nullptr;
    }

    std::istringstream positionStr(jsonLight["position"].get<std::string>());
    std::istringstream directionStr(jsonLight["direction"].get<std::string>());
    std::istringstream colourStr(jsonLight["colour"].get<std::string>());

    float x, y, z;

    // Get position
    positionStr >> x >> y >> z;
    const vec3 position(x, y, z);

    // Get direction
    directionStr >> x >> y >> z;
    const vec3 direction(x, y, z);

    // Get colour
    colourStr >> x >> y >> z;
    const vec3 colour(x, y, z);

    // Get intensity
    const float intensity = jsonLight["intensity"];

    return new Light(position, direction, colour, intensity);
}

mat4 Light::getView() const {
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    if (glm::abs(glm::dot(direction, up)) > 0.999f) {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    return glm::lookAt(position, position + direction, up);
}

void Light::setDepthMapIndex(const int depthMapIndex) {
    this->depthMapIndex = depthMapIndex;
}

void Light::setLightSpaceMatrix(const mat4 &proj) {
    lightSpaceMatrix = proj * getView();
}

mat4 Light::getLightSpaceMatrix() const {
    return lightSpaceMatrix;
}