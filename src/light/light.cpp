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
    std::istringstream directionStr(jsonLight["position"].get<std::string>());
    std::istringstream colourStr(jsonLight["position"].get<std::string>());

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

mat4 Light::get_view_matrix() const {
    // TODO has to be better way, maybe move this into space
    const vec3 right = glm::normalize(glm::cross(direction, vec3(0.0f, 1.0f, 0.0f)));
    const vec3 up    = glm::normalize(glm::cross(right, direction));

    return glm::lookAt(position, position + direction, up);
}

void Light::setDepthMapIndex(const int depthMapIndex) {
    this->depthMapIndex = depthMapIndex;
}