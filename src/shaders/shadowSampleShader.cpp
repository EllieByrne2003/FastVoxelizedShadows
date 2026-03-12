#include "shaders.hpp"

#include <string>
#include <mutex>
#include <map>

#include <GL/glew.h>
#include <GL/gl.h>

#include "../scene/scene.hpp"

#include "../utils/utils.hpp"

#define SHADER_DIR std::string("res/shaders")

ShadowSampleShader::ShadowSampleShader(const std::shared_ptr<const GLuint> &id) : Shader(id) {
    lightSpaceMatrixLoc = glGetUniformLocation(*id, "lightSpaceMatrix");
}

ShadowSampleShader::~ShadowSampleShader() {

}

ShadowSampleShader * ShadowSampleShader::readShader(const json &jsonShader) {
    static std::mutex cacheMutex;
    static std::map<std::string, ShadowSampleShader> shaderCache;

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
        return new ShadowSampleShader(shaderCache.at(key));
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

        shaderCache.emplace(key, ShadowSampleShader(ID));
        return new ShadowSampleShader(shaderCache.at(key));
    }
}

void ShadowSampleShader::passShaderUniforms() const {

}

void ShadowSampleShader::passModelUniforms(const Model *const model, const mat4 &trueMatrix) const {
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &trueMatrix[0][0]);
}

void ShadowSampleShader::passSceneUniforms(const Scene *const scene) const {
    const mat4 view = scene->getView();
    const mat4 proj = scene->getProj();

    const mat4 lightSpaceMatrix = scene->getLightMatrix();

    glUniformMatrix4fv(viewLoc,             1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc,             1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
}