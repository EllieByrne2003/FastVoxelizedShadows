#pragma once

#include <memory>

#include <glm/glm.hpp>
using namespace glm;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Model;

typedef unsigned int GLuint;

class Shader {
private:
    const std::shared_ptr<const GLuint> id;

    GLuint mvpLoc;

protected:

public:
    Shader(const std::shared_ptr<const GLuint> &id);
    ~Shader();

    static Shader * readShader(const json &jsonShader);

    void bind() const;
    void passMVP(const mat4 &mvp) const;
    virtual void passShaderUniforms() const;
    virtual void passModelUniforms(const Model *const model) const;
};