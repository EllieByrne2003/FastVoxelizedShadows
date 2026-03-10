#pragma once

#include <glm/glm.hpp>
using namespace glm;

enum Direction {
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT,
    UP,
    DOWN
};

class Camera {
private:
    vec3 position;
    float yaw;
    float pitch;

    const float speed = 1.0f;
    const float mouseSensitivity = 1.0f;

protected:

public:
    Camera(const vec3 &position = vec3(0.0f));
    ~Camera();

    vec3 forward(const bool worldUp = false) const;
    vec3 right(const bool worldUp = false) const;
    vec3 up(const bool worldUp = false) const;

    mat4 get_view_matrix() const;

    void move(const Direction direction, const float time);
    void look(const float deltaX, const float deltaY);

    vec3 getPos() const;
};