#pragma once

#include <memory>
#include <mutex>

#include <glm/glm.hpp>
using namespace glm;

class Shader;

class Renderer {
private:
    Renderer();
    ~Renderer();

    std::mutex glMutex;

    mat4 view = mat4(1.0f);
    mat4 proj = mat4(1.0f);

protected:

public:
    static Renderer & getInstance();

    Renderer(Renderer const &) = delete;
    void operator=(Renderer const &) = delete;

    // Functions to synchronize GPU access
    void claimContext();
    void releaseContext();

    void setView(const mat4 &view);
    void setProj(const mat4 &proj);

    const mat4 & getView();
    const mat4 & getProj();

    mat4 getVP() const;
    mat4 getMVP(const mat4 &model) const;
};