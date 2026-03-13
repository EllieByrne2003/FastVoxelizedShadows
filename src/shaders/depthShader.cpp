#include "shaders.hpp"

#include <string>
#include <mutex>
#include <map>

#include <GL/glew.h>
#include <GL/gl.h>

#include "../scene/scene.hpp"
#include "../light/light.hpp"

#include "../utils/utils.hpp"

#define SHADER_DIR std::string("res/shaders")

DepthShader::DepthShader(const std::shared_ptr<const GLuint> &id) : Shader(id) {
    positionLoc  = glGetUniformLocation(*id, "position");
    directionLoc = glGetUniformLocation(*id, "direction");
    coneAngleLoc = glGetUniformLocation(*id, "coneAngle");
}

DepthShader::~DepthShader() {

}

DepthShader * DepthShader::readShader(const json &jsonShader) {
    static std::mutex cacheMutex;
    static std::map<std::string, DepthShader> shaderCache;

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
        return new DepthShader(shaderCache.at(key));
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

        shaderCache.emplace(key, DepthShader(ID));
        return new DepthShader(shaderCache.at(key));
    }
}

void DepthShader::passShaderUniforms() const {

}

void DepthShader::passModelUniforms(const MeshModel *const model, const mat4 &trueMatrix) const {
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &trueMatrix[0][0]);
}

void DepthShader::passSceneUniforms(const Scene *const scene) const {
    // Nothing to do here
}

void DepthShader::passLightUniforms(const Light &light, const mat4 &proj) const {
    const mat4 view = light.getView();
    // TODO can just get both the view and proj as one lightSpace matrix

    const vec3  position  = light.getPosition();
    const vec3  direction = light.getDirection();
    const float coneangle = light.getConeAngle();

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);

    glUniform3f(positionLoc,  position.x,  position.y,  position.z);
    glUniform3f(directionLoc, direction.x, direction.y, direction.z);
    glUniform1f(coneAngleLoc, coneangle);
}
