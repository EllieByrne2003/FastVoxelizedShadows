#pragma once

#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Model;

class Scene {
private:
    std::string name;
    std::string description;

    std::vector<Model *> models;
protected:

public:
    Scene(const std::string &name, const std::string &description);
    ~Scene();

    static Scene * readScene(const json &jsonScene);
    static Scene * readScene(const std::string &sceneName);
    void draw();

    void addModel(Model *model);
};