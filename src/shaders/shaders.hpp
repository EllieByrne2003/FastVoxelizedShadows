#pragma once

#include <memory>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

typedef unsigned int GLuint;

class Shader {
private:
    const std::shared_ptr<const GLuint> id;

protected:

public:
    Shader(const std::shared_ptr<const GLuint> &id);
    ~Shader();

    static Shader * readShader(const json &jsonShader);

    void bind() const;
};