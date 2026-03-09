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

    return scene;
}

Scene * Scene::readScene(const std::string &sceneName) {
    const std::string scenePath = SCENE_DIR + "/" + sceneName;

    std::ifstream scene(scenePath);
    json jsonScene = json::parse(scene);
    return readScene(jsonScene);
}

void Scene::draw() {
    for(Model *model : models) {
        model->draw();
    }
}

void Scene::addModel(Model *model) {
    models.push_back(model);
}