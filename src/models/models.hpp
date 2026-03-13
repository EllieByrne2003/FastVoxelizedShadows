#pragma once

#include <string>
#include <vector>
#include <memory>
#include <istream>

#include <glm/glm.hpp>
using namespace glm;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "../bounds/bounds.hpp"

typedef unsigned int GLuint;

class DepthShader;
class Animation;
class Renderer;
class Texture;
class Shader;
class Scene;
class Light;
class Mesh;

class Model {
private:
    vec3 basePosition;
    vec3 baseRotation;
    vec3 baseScale;

protected:
    vec3 position;
    vec3 rotation;
    vec3 scale;

    std::vector<Animation *> animations;
    int currentAnimation = 0;

public:
    Model(const vec3 &position, const vec3 &rotation, const vec3 &scale);
    virtual ~Model();

    static Model * readModel(const json &jsonModel);

    vec3 getBasePosition() const;
    vec3 getBaseRotation() const;
    vec3 getBaseScale() const;

    void setBasePosition(const vec3 &value);
    void setBaseRotation(const vec3 &value);
    void setBaseScale(const vec3 &value);

    vec3 getPosition() const;
    vec3 getRotation() const;
    vec3 getScale() const;

    void setPosition(const vec3 &value);
    void setRotation(const vec3 &value);
    void setScale(const vec3 &value);

    void resetPosition();
    void resetRotation();
    void resetScale();

    void translateXYZ(const vec3 &value);
    void translateX(const float value);
    void translateY(const float value);
    void translateZ(const float value);

    void rotateXYZ(const vec3 &value);
    void rotateX(const float value);
    void rotateY(const float value);
    void rotateZ(const float value);

    void scaleXYZ(const vec3 &value);
    void scaleX(const float value);
    void scaleY(const float value);
    void scaleZ(const float value);

    mat4 createLocalModelMatrix() const;

    void addAnimation(Animation *animation);

    // To be implemented by derived classes
    virtual bool isStatic()  const = 0;
    virtual bool isDynamic() const = 0;

    virtual void update(const float deltaTime) = 0;

    // virtual void setShader(const std::shared_ptr<Shader> &shader) = 0;

    virtual void draw(const Scene *const scene, const mat4 &parentModel = mat4(1.0f)) const = 0;
    virtual void drawDepths(const Scene *const scene, const Light &light, const bool drawEntry, const mat4 &parentModel = mat4(1.0f)) const = 0; // TODO pass light proj since it should be made once for whole scene
    // virtual void draw() const = 0;

    virtual void expandBounds(Bounds &bounds, const mat4 &view, const mat4 &proj, const mat4 &parentModel = mat4(1.0f)) const = 0;
};

class MeshModel : public Model {
private:
    const Mesh *mesh;

    const Texture *texture;

    const Shader      *shader;
    const DepthShader *depthShader;

protected:

public:
    MeshModel(const vec3 &position, const vec3 &rotation, const vec3 &scale, const Shader *shader, const DepthShader *depthShader, const Mesh *mesh, const Texture *texture);
    ~MeshModel() override;

    // Overriden functions
    bool isStatic()  const override final;
    bool isDynamic() const override final;

    // void setShader(const std::shared_ptr<Shader> &shader) override final;

    void update(const float deltaTime) override final;

    void draw(const Scene *const scene, const mat4 &parentModel = mat4(1.0f)) const override final;
    void drawDepths(const Scene *const scene, const Light &light, const bool drawEntry, const mat4 &parentModel = mat4(1.0f)) const override final;

    void expandBounds(Bounds &bounds, const mat4 &view, const mat4 &proj, const mat4 &parentModel = mat4(1.0f)) const override final;

    // Class specific functions
    // void bindShader() const;
    void bindTexture(const GLuint loc) const;
};

class CompositeModel : public Model {
private:
    std::vector<Model *> models;

protected:

public:
    CompositeModel(const vec3 &position, const vec3 &rotation, const vec3 &scale);
    ~CompositeModel() override;

    // Overriden functions
    virtual bool isStatic()  const final;
    virtual bool isDynamic() const final;

    // virtual void setShader(const std::shared_ptr<Shader> &shader) final;

    virtual void update(const float deltaTime) final;

    void draw(const Scene *const scene, const mat4 &parentModel = mat4(1.0f)) const override final;
    void drawDepths(const Scene *const scene, const Light &light, const bool drawEntry, const mat4 &parentModel = mat4(1.0f)) const override final;

    void expandBounds(Bounds &bounds, const mat4 &view, const mat4 &proj, const mat4 &parentModel = mat4(1.0f)) const override final;

    // Class specific functions
    void addModel(Model *model);
};