#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Model;

class Animation {
private:
    float currentTime = 0.0f;
    const float totalTime;

    const vec3 translation;
    const vec3 rotation;
    const vec3 scale;

protected:

public:
    Animation(const float totalTime, const vec3 &translation, const vec3 &rotation, const vec3 &scale);
    ~Animation();

    static Animation * readAnimation(const json &jsonAnimation);

    float update(Model *model, const float deltaTime);
    bool isComplete() const;
    void reset();
};