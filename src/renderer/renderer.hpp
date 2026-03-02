#pragma once

#include <memory>

class Shader;

class Renderer {
private:
    std::shared_ptr<Shader> shadowShader;

protected:

public:
    Renderer();
    ~Renderer();

    void bindShadowShader();
};