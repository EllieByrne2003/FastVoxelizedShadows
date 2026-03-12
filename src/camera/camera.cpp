#include "camera.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

Camera::Camera(const vec3 &position) {
    this->position = position;
}

Camera::~Camera() {

}

vec3 Camera::forward(const bool worldUp) const {
    const float forwardX = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    const float forwardY = worldUp ? 0.0f : sin(glm::radians(pitch));
    const float forwardZ = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    return vec3(forwardX, worldUp ? 0.0f : forwardY, forwardZ);
}

vec3 Camera::right(const bool worldUp) const {
    const float rightX = -sin(glm::radians(yaw));
    const float rightY =  0.0f;
    const float rightZ =  cos(glm::radians(yaw));

    return vec3(rightX, worldUp ? 0.0f : rightY, rightZ);
}

vec3 Camera::up(const bool worldUp) const {
    const float upX = -cos(glm::radians(yaw)) * sin(glm::radians(pitch));
    const float upY =  cos(glm::radians(pitch));
    const float upZ = -sin(glm::radians(yaw)) * sin(glm::radians(pitch));

    return vec3(worldUp ? 0.0f : upX, worldUp ? 1.0f : upY, worldUp ? 0.0f : upZ);
}

void Camera::move(const Direction direction, const float time) {
    if(direction == FORWARD)  position += forward(true) * time * speed;
    if(direction == BACKWARD) position -= forward(true) * time * speed;
    if(direction == RIGHT)    position += right(true)   * time * speed;
    if(direction == LEFT)     position -= right(true)   * time * speed;
    if(direction == UP)       position += up(true)      * time * speed;
    if(direction == DOWN)     position -= up(true)      * time * speed;
}

void Camera::look(const float deltaX, const float deltaY) {
    yaw   -= deltaX * mouseSensitivity;
    pitch += deltaY * mouseSensitivity;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);
}

void Camera::resizeView(const int width, const int height) {
    this->width  = width;
    this->height = height;
}

vec3 Camera::getPos() const {
    return position;
}

mat4 Camera::getView() const {
    return glm::lookAt(position, position + forward(), up());
}

mat4 Camera::getProj() const {
    // TODO have changable FOV or pass to function
    return glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);
}

void Camera::setViewPort() const {
    glViewport(0, 0, width, height);
}