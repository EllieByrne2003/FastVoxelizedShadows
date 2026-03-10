#include "scene.hpp"

#include <fstream>

#include "../models/models.hpp"

#define SCENE_DIR std::string("res/scenes")

Scene::Scene(const std::string &name, const std::string &description) {
    this->name = name;
    this->description = description;
}

Scene::~Scene() {
    for(Model *model : models) {
        delete model;
    }
}


Scene * Scene::readScene(const json &jsonScene) {
    std::string name;
    if(jsonScene.contains("name")) {
        name = jsonScene["name"];
    } else {
        name = "un-named";
    }

    std::string description;
    if(jsonScene.contains("description")) {
        description = jsonScene["description"];
    } else {
        description = "";
    }

    Scene *scene = new Scene(name, description);

    if(jsonScene.contains("models")) {
        for(const json &jsonModel : jsonScene["models"]) {
            Model *model = Model::readModel(jsonModel);

            if(model != nullptr) {
                scene->addModel(model);
            }
        }
    }

    if(jsonScene.contains("lights")) {
        for(const json &jsonLight : jsonScene["lights"]) {
            Light *light = Light::readLight(jsonLight);

            if(light != nullptr) {
                scene->addLight(light);
            }
        }
    }

    return scene;
}

Scene * Scene::readScene(const std::string &sceneName) {
    const std::string scenePath = SCENE_DIR + "/" + sceneName;

    std::ifstream scene(scenePath);
    json jsonScene = json::parse(scene);
    return readScene(jsonScene);
}

void Scene::draw() {
    Renderer::getInstance().setView(camera.get_view_matrix());

    for(Model *model : models) {
        model->draw(this);
    }
}

void Scene::drawDepths(const bool drawEntry) {
    for(Model *model : models) {
        model->draw();
    }
}

void Scene::addModel(Model *model) {
    models.push_back(model);
}

void Scene::addLight(Light *light) {
    lights.push_back(light);
}

void Scene::move(const Direction direction, const float time) {
    camera.move(direction, time);

    Renderer::getInstance().setView(camera.get_view_matrix());
}

void Scene::look(const float deltaX, const float deltaY) {
    camera.look(deltaX, deltaY);

    Renderer::getInstance().setView(camera.get_view_matrix());
}

vec3 Scene::getCameraPos() const {
    return camera.getPos();
}

// TODO temporary
mat4 Scene::getLightMatrix() const {
    const mat4 proj = glm::ortho(0.0f, 1024.0f, 0.0f, 1024.0f, 0.1f, 1000.0f);
    const mat4 view = lights[0]->get_view_matrix();

    return proj * view;
}