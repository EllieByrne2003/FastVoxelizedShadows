#include "models.hpp"

#include "../animation/animation.hpp"

CompositeModel::CompositeModel(const vec3 &position, const vec3 &rotation, const vec3 &scale) :
    Model(position, rotation, scale) 
{

}

CompositeModel::~CompositeModel() {
    for(Model *model : models) {
        delete model;
    }
}

bool CompositeModel::isStatic() const {
    for(Model *const model : models) {
        if(!model->isStatic()) {
            return false;
        }
    }

    return animations.empty();
}

bool CompositeModel::isDynamic() const {
    for(Model *const model : models) {
        if(!model->isDynamic()) {
            return false;
        }
    }

    return !animations.empty();
}

// void CompositeModel::setShader(const std::shared_ptr<Shader> &shader) {
//     for(Model *model : models) {
//         model->setShader(shader);
//     }
// }

void CompositeModel::update(const float deltaTime) {
    if(isStatic()) {
        return;
    }

    Animation *animation = animations[currentAnimation];
    const float timePassed = animation->update(this, deltaTime);

    for(Model *model : models) {
        // If timePassed is less than deltaTime we can't run the full deltaTime
        // for child objects and then recurse, it'll cause child objects to 
        // experience too much time
        model->update(timePassed);
    }

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

void CompositeModel::draw(const Scene *const scene, const mat4 &parentModel) const {
    // Create model matrix and combine with parentModel matrix
    const mat4 localModel = createLocalModelMatrix();
    const mat4 trueModel = parentModel * localModel;

    for(Model *const model : models) {
        model->draw(scene, trueModel);
    }
}

void CompositeModel::drawDepths(const Scene *const scene, const Light &light, const bool drawEntry, const mat4 &parentModel) const {
    // Create model matrix and combine with parentModel matrix
    const mat4 localModel = createLocalModelMatrix();
    const mat4 trueModel = parentModel * localModel;

    for(Model *const model : models) {
        model->drawDepths(scene, light, drawEntry, trueModel);
    }
}

void CompositeModel::addModel(Model *model) {
    models.push_back(model);
}