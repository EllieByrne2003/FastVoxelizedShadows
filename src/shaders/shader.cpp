#include "shaders.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

Shader::Shader(const std::shared_ptr<const GLuint> &id) : id(id) {

}

Shader::~Shader() {

}

void Shader::bind() const {
    glUseProgram(*id);
}