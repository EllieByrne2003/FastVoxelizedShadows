#include "models.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include "../animation/animation.hpp"
#include "../renderer/renderer.hpp"
#include "../shaders/shaders.hpp"
#include "../texture/texture.hpp"
#include "../scene/scene.hpp"
#include "../mesh/mesh.hpp"

MeshModel::MeshModel(const vec3 &position, const vec3 &rotation, const vec3 &scale, const Shader *shader, const DepthShader *depthShader, const Mesh *mesh, const Texture *texture) :
    Model(position, rotation, scale)
{
    this->mesh = mesh;

    this->texture = texture;

    this->shader = shader;
    this->depthShader = depthShader;
}

// TODO have some null checking here
MeshModel::~MeshModel() {
    delete mesh;

    delete texture;

    delete shader;
    delete depthShader;
}

bool MeshModel::isStatic() const {
    return animations.empty();
}

bool MeshModel::isDynamic() const {
    return !animations.empty();
}

// void MeshModel::setShader(const std::shared_ptr<Shader> &shader) {
//     this->shader = shader;
// }

void MeshModel::update(const float deltaTime) {
    if(isStatic()) {
        return;
    }

    Animation *animation = animations[currentAnimation];
    const float timePassed = animation->update(this, deltaTime);

    // Progress to next animation or start chain over again
    if(animation->isComplete()) {
        currentAnimation++;
        animation->reset();

        if(currentAnimation >= animations.size()) {
            currentAnimation = 0;

            resetPosition();
            resetRotation();
            resetScale();
        }
    }

    // If the animation didn't use all the time, recurse to use the remainder
    if(timePassed < deltaTime) {
        update(deltaTime - timePassed);
    }
}

void MeshModel::draw(const Scene *const scene, const mat4 &parentModel) const {
    // Create model matrix and combine with parentModel matrix then make mvp
    const mat4 localModel = createLocalModelMatrix();
    const mat4 trueModel = parentModel * localModel;

    // Bind the shader and setup the uniforms
    shader->bind();
    shader->passShaderUniforms();
    shader->passModelUniforms(this, trueModel);
    shader->passSceneUniforms(scene);

    // Bind textures, normal maps, etc if present
    // TODO implement above
    // texture->bind(0);

    // draw mesh
    mesh->draw();
}

void MeshModel::drawDepths(const Scene *const scene, const Light *const light, const bool drawEntry, const mat4 &parentModel) const {
    // Create model matrix and combine with parentModel matrix then make mvp
    const mat4 localModel = createLocalModelMatrix();
    const mat4 trueModel = parentModel * localModel;

    // Bind the shader, textures, normal maps etc
    depthShader->bind();
    depthShader->passShaderUniforms();
    depthShader->passModelUniforms(this, trueModel);
    depthShader->passSceneUniforms(scene);
    depthShader->passLightUniforms(light, scene->getProj(light));

    // draw mesh
    mesh->draw();
}