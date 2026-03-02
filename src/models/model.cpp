#include "models.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "../animation/animation.hpp"
#include "../shaders/shaders.hpp"
#include "../utils/utils.hpp"

Model::Model(const vec3 &position, const vec3 &rotation, const vec3 &scale) {
    this->position = position;
    this->rotation = rotation;
    this->scale    = scale;
}

Model::~Model() {
    for(Animation *animation : animations) {
        delete animation;
    }
}

Model * Model::readModel(const json &jsonModel) {
    if(!jsonModel.contains("position")) {
        return nullptr;
    }


    if(!jsonModel.contains("rotation")) {
        return nullptr;
    }

    if(!jsonModel.contains("scale")) {
        return nullptr;
    }

    // All needed types
    std::istringstream positionStr(jsonModel["position"].get<std::string>());
    std::istringstream rotationStr(jsonModel["rotation"].get<std::string>());
    std::istringstream scaleStr(jsonModel["scale"].get<std::string>());

    float x, y, z;

    // Get position
    positionStr >> x >> y >> z;
    const vec3 position(x, y, z);

    // Get rotation
    rotationStr >> x >> y >> z;
    const vec3 rotation(x, y, z);

    // Get scale
    scaleStr >> x >> y >> z;
    const vec3 scale(x, y, z);

    // Make model the correct type (if one exists)
    Model *model = nullptr;
    if(jsonModel.contains("mesh") && jsonModel.contains("models")) {
        // Can't have both
        return nullptr;
    } else if(jsonModel.contains("mesh")) {
        Shader *shader = Shader::readShader(jsonModel["shader"]);
        if(shader == nullptr) {
            return nullptr;
        }

        Shader *shadowShader = Shader::readShader(jsonModel["shadowShader"]);
        if(shadowShader == nullptr) {
            delete shader;

            return nullptr;
        }

        GLuint VAO, VBO, EBO;
        const int indicesCount = readMesh(jsonModel["mesh"], VAO, VBO, EBO);

        if(indicesCount <= 0) {
            delete shader;
            delete shadowShader;

            return nullptr;
        }

        model = new MeshModel(position, rotation, scale, shader, shadowShader, VAO, VBO, EBO, indicesCount);
    } else if(jsonModel.contains("models")) {
        model = new CompositeModel(position, rotation, scale);

        for(const json &jsonChildModel : jsonModel["models"]) {
            Model *childModel = Model::readModel(jsonChildModel);

            if(childModel != nullptr) {
                static_cast<CompositeModel *>(model)->addModel(childModel);
            }
        }
    } else {
        // Can't have none
        return nullptr;
    }

    // Load animations
    if(jsonModel.contains("animations")) {
        for(const json &jsonAnimation : jsonModel["animations"]) {
            Animation *animation = Animation::readAnimation(jsonAnimation);

            if(animation != nullptr) {
                model->addAnimation(animation);
            }
        }
    }
    
    return model;
}

vec3 Model::getBasePosition() const {
    return basePosition;
}

vec3 Model::getBaseRotation() const {
    return baseRotation;
}

vec3 Model::getBaseScale() const {
    return baseScale;
}

vec3 Model::getPosition() const {
    return position;
}

vec3 Model::getRotation() const {
    return rotation;
}

vec3 Model::getScale() const {
    return scale;
}

void Model::resetPosition() {
    position = basePosition;
}

void Model::resetRotation() {
    rotation = baseRotation;
}

void Model::resetScale() {
    scale = baseScale;
}

void Model::translateXYZ(const vec3 &value) {
    position += value;
}

void Model::translateX(const float value) {
    position.x += value;
}

void Model::translateY(const float value) {
    position.y += value;
}

void Model::translateZ(const float value) {
    position.z += value;
}

void Model::rotateXYZ(const vec3 &value) {
    rotation += value;
}

void Model::rotateX(const float value) {
    rotation.x = value;
}

void Model::rotateY(const float value) {
    rotation.y = value;
}

void Model::rotateZ(const float value) {
    rotation.z = value;
}

void Model::scaleXYZ(const vec3 &value) {
    scale *= value;
}

void Model::scaleX(const float value) {
    scale.x *= value;
}

void Model::scaleY(const float value) {
    scale.y *= value;
}

void Model::scaleZ(const float value) {
    scale.z *= value;
}

mat4 Model::createLocalModelMatrix() const {
    mat4 model(1.0f);

    model = glm::translate(model, position);

    model = glm::rotate(model, glm::radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, scale);

    return model;
}

void Model::addAnimation(Animation *animation) {
    animations.push_back(animation);
}