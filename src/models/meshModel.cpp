#include "models.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include "../animation/animation.hpp"
#include "../renderer/renderer.hpp"
#include "../shaders/shaders.hpp"
#include "../texture/texture.hpp"
#include "../mesh/mesh.hpp"

MeshModel::MeshModel(const vec3 &position, const vec3 &rotation, const vec3 &scale, const Shader *shader, const Shader *shadowShader, const Mesh *mesh, const Texture *texture) :
    Model(position, rotation, scale)
{
    this->mesh = mesh;

    this->texture = texture;

    this->shader = shader;
    this->shadowShader = shadowShader;
}

// TODO have some null checking here
MeshModel::~MeshModel() {
    delete mesh;

    delete texture;

    delete shader;
    delete shadowShader;
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
    const Renderer &renderer = Renderer::getInstance();

    // Create model matrix and combine with parentModel matrix then make mvp
    const mat4 localModel = createLocalModelMatrix();
    const mat4 trueModel = parentModel * localModel;
    const mat4 mvp = renderer.getMVP(trueModel);

    // Bind the shader and setup the uniforms
    shader->bind();
    shader->passMVP(mvp); // TODO this can be done in model uniform function
    shader->passShaderUniforms();
    shader->passModelUniforms(this, trueModel);
    // shader->passSceneUniforms(scene);

    // Bind textures, normal maps, etc if present
    // TODO implement above
    texture->bind(0);

    // draw mesh
    mesh->draw();
}

void MeshModel::drawDepths(const Scene *const scene, const bool drawEntry, const mat4 &parentModel) const {
    const Renderer &renderer = Renderer::getInstance();

    // Create model matrix and combine with parentModel matrix then make mvp
    const mat4 localModel = createLocalModelMatrix();
    const mat4 trueModel = parentModel * localModel;
    const mat4 mvp = renderer.getMVP(trueModel);

    // Bind the shader, textures, normal maps etc
    shadowShader->bind();
    shadowShader->passMVP(mvp);
    shadowShader->passShaderUniforms();
    shadowShader->passModelUniforms(this, trueModel);
    shader->passSceneUniforms(scene);

    // draw mesh
    mesh->draw();
}