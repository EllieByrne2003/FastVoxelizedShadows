#pragma once

#include <memory>

#include <glm/glm.hpp>
using namespace glm;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class MeshModel;
class Model;
class Scene;
class Light;

typedef unsigned int GLuint;

class Shader {
private:
    const std::shared_ptr<const GLuint> id;

protected:
    GLuint modelLoc;
    GLuint viewLoc;
    GLuint projLoc;

public:
    Shader(const std::shared_ptr<const GLuint> &id);
    virtual ~Shader();

    static Shader * readShader(const json &jsonShader);

    void bind() const;
    virtual void passShaderUniforms() const;
    virtual void passModelUniforms(const MeshModel *const model, const mat4 &trueMatrix) const = 0;
    virtual void passSceneUniforms(const Scene *const scene) const = 0;
};

class DepthShader : public Shader {
private:

protected:

public:
    DepthShader(const std::shared_ptr<const GLuint> &id);
    ~DepthShader() override;

    static DepthShader * readShader(const json &jsonShader);

    void passShaderUniforms() const override;
    void passModelUniforms(const MeshModel *const model, const mat4 &trueMatrix) const override;
    void passSceneUniforms(const Scene *const scene) const override;

    void passLightUniforms(const Light &light, const mat4 &proj) const;
};

class ShadowSampleShader : public Shader {
private:
    GLuint lightCountLoc;
    GLuint depthMapsLoc;
    GLuint cameraPosLoc;

    GLuint textureMapLoc;

protected:

public:
    ShadowSampleShader(const std::shared_ptr<const GLuint> &id);
    ~ShadowSampleShader() override;

    static ShadowSampleShader * readShader(const json &jsonShader);

    void passShaderUniforms() const override;
    void passModelUniforms(const MeshModel *const model, const mat4 &trueMatrix) const override;
    void passSceneUniforms(const Scene *const scene) const override;
};