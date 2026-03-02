#include "animation.hpp"

#include "../models/models.hpp"

Animation::Animation(const float totalTime, const vec3 &translation, const vec3 &rotation, const vec3 &scale) :
    totalTime(totalTime), translation(translation), rotation(rotation), scale(scale) {

}

Animation::~Animation() {

}

float Animation::update(Model *model, const float deltaTime) {
    const float timeLeft = totalTime - currentTime;

    // We only want to run animations for their totalTime, not more
    const float timePassed = deltaTime > timeLeft ? timeLeft : deltaTime;
    const float ratio = timePassed / totalTime;

    model->translateXYZ(translation * ratio);
    model->rotateXYZ(rotation * ratio);
    model->scaleXYZ(scale * ratio);

    currentTime += timePassed;
    
    return timePassed;
}

bool Animation::isComplete() const {
    return currentTime >= totalTime;
}

void Animation::reset() {
    currentTime = 0;
}