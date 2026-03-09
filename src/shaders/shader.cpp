#include "shaders.hpp"

#include <string>
#include <mutex>
#include <map>

#include <GL/glew.h>
#include <GL/gl.h>

#include "../utils/utils.hpp"

#define SHADER_DIR std::string("res/shaders")

Shader::Shader(const std::shared_ptr<const GLuint> &id) : id(id) {
    mvpLoc = glGetUniformLocation(*id, "mvp");
}

Shader::~Shader() {

}

Shader * Shader::readShader(const json &jsonShader) {
    static std::mutex cacheMutex;
    static std::map<std::string, Shader> shaderCache;

    if(!jsonShader.contains("vertex")) {
        return nullptr;
    }

    if(!jsonShader.contains("fragment")) {
        return nullptr;
    }

    std::unique_lock lock(cacheMutex);

    // TODO shaders will have types for different rendering later, should be part of key and return diff types
    // TODO check json has fields
    const std::string vertexName   = jsonShader["vertex"];
    const std::string fragmentName = jsonShader["fragment"];
    const std::string key          = "v:" + vertexName + ", f:" + fragmentName;

    if(shaderCache.find(key) != shaderCache.end()) {
        return new Shader(shaderCache.at(key));
    } else {
        const std::string vertexPath   = SHADER_DIR + "/" + vertexName;
        const std::string fragmentPath = SHADER_DIR + "/" + fragmentName;

        GLuint *id = new GLuint;
        *id = createShader(vertexPath, fragmentPath);

        if(*id == 0) {
            delete id;

            return nullptr;
        }

        std::shared_ptr<GLuint> ID(id, deleteShaderProgram);

        shaderCache.emplace(key, Shader(ID));
        return new Shader(shaderCache.at(key));
    }
}

void Shader::bind() const {
    glUseProgram(*id);
}

void Shader::passMVP(const mat4 &mvp) const {
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
}

void Shader::passShaderUniforms() const {

}

void Shader::passModelUniforms(const Model *const model) const {
    
}