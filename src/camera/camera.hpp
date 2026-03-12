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
    float yaw = 0.0f;
    float pitch = -89.0f;

    const float speed = 5.0f;
    const float mouseSensitivity = 1.0f;

    // Projection stuff
    int width = 1024;
    int height = 1024;
    // float near;
    // float far;

protected:

public:
    Camera(const vec3 &position = vec3(0.0f, 0.0f, 0.0f));
    ~Camera();

    vec3 forward(const bool worldUp = false) const;
    vec3 right(const bool worldUp = false) const;
    vec3 up(const bool worldUp = false) const;

    // mat4 get_view_matrix() const;

    void move(const Direction direction, const float time);
    void look(const float deltaX, const float deltaY);
    void resizeView(const int width, const int height);

    vec3 getPos() const;

    mat4 getView() const;
    mat4 getProj() const;

    void setViewPort() const;
};