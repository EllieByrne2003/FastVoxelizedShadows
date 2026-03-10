#include "shaders.hpp"

#include <string>
#include <mutex>
#include <map>

#include <GL/glew.h>
#include <GL/gl.h>

#include "../scene/scene.hpp"

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

void Shader::passModelUniforms(const Model *const model, const mat4 &trueMatrix) const {
    glUniformMatrix4fv(glGetUniformLocation(*id, "model"), 1, GL_FALSE, &trueMatrix[0][0]);
}

void Shader::passSceneUniforms(const Scene *const scene) const {
    // Pass light data
    // TODO this should be in a subclass of shadow shader
    const mat4 lightMat = scene->getLightMatrix();

    glUniformMatrix4fv(glGetUniformLocation(*id, "mvp"), 1, GL_FALSE, &lightMat[0][0]);
    glUniform3fv(glGetUniformLocation(*id, "cameraPos"), 1, &scene->getCameraPos()[0]);
}