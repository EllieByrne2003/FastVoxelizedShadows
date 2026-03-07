#include "renderer.hpp"

Renderer::Renderer() {

}

Renderer::~Renderer() {

}

Renderer & Renderer::getInstance() {
    static Renderer renderer;
    return renderer;
}

void Renderer::claimContext() {
    glMutex.lock();
}

void Renderer::releaseContext() {
    glMutex.lock();
}

void Renderer::setView(const mat4 &view) {
    this->view = view;
}

void Renderer::setProj(const mat4 &proj) {
    this->proj = proj;
}

const mat4 & Renderer::getView() {
    return view;
}

const mat4 & Renderer::getProj() {
    return proj;
}

mat4 Renderer::getVP() const {
    return proj * view;
}

mat4 Renderer::getMVP(const mat4 &model) const {
    return proj * view * model;
}