#pragma once

#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "../camera/camera.hpp"

class Model;
class Light;

class Scene {
private:
    std::string name;
    std::string description;

    std::vector<Model *> models;
    std::vector<Light *> lights;

    Camera camera; // TODO add this to reading scenes
protected:

public:
    Scene(const std::string &name, const std::string &description);
    ~Scene();

    static Scene * readScene(const json &jsonScene);
    static Scene * readScene(const std::string &sceneName);

    void draw();
    void drawDepths(const Light *const light, const bool drawEntry);

    void setupLights(const bool voxelize);

    void addModel(Model *model);
    void addLight(Light *light);

    void move(const Direction direction, const float time);
    void look(const float deltaX, const float deltaY);
    void resizeView(const int width, const int height);

    mat4 getView() const;
    mat4 getProj() const;

    mat4 getProj(const Light *const light) const;

    vec3 getCameraPos() const;

    // TODO temporary
    mat4 getLightMatrix() const;
};