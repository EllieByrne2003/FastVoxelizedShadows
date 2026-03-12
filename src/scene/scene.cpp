#include "scene.hpp"

#include <fstream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "../light/light.hpp"
#include "../models/models.hpp"
#include "../texture/texture.hpp"
#include "../renderer/renderer.hpp"

#define SCENE_DIR std::string("res/scenes")

Scene::Scene(const std::string &name, const std::string &description) {
    this->name = name;
    this->description = description;
}

Scene::~Scene() {
    for(Model *model : models) {
        delete model;
    }

    for(Light *light : lights) {
        delete light;
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
    camera.setViewPort(); // Sets the viewport to the size of the scenes camera

    for(Model *model : models) {
        model->draw(this);
    }
}

void Scene::drawDepths(const Light *const light, const bool drawEntry) {
    for(Model *model : models) {
        model->drawDepths(this, light, drawEntry);
    }
}

void Scene::setupLights(const bool voxelize) {
    // TODO do this, for now just bodge it

    Renderer &renderer = Renderer::getInstance();

    // Create texture, bind to view buffer later
    Texture *depth = Texture::createTexture(16*1024, 16*1024, 1);

    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    // Attach the depth texture to the FBO
    depth->bindToFrameBuffer();

    // Explicitly tell OpenGL that we will not render any color data
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Check if the framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // Handle error
        // std::cerr << "Something wrong with depths" << std::endl;
    }

    // // Unbind the FBO for now
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Take first light, setup renderer
    const Light *const light = lights[0];
    renderer.setView(light->getView());
    renderer.setProj(glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 20.0f)); // TODO figure out proper values


    glViewport(0, 0, 16*1024, 16*1024);   // width/height = depth texture dimensions
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Draw depths
    this->drawDepths(lights[0], true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    depth->bind(1);

    glViewport(0, 0, 1024, 1024);   // width/height = depth texture dimensions


    // float *entryDepths = new float[1024 * 1024 * 4];
    // glReadPixels(0, 0, 1024, 1024, GL_DEPTH_COMPONENT, GL_FLOAT, entryDepths);

    // stbi_write_bmp("depth.bmp", 1024, 1024, 4, entryDepths);

    // // Bind texture to slot 1
    // // depth->unbindFromFrameBuffer(); // TODO preserve width and height?
    // depth->bind(1);

    if(voxelize) {
        // Spin up some threads (max of like 8 or something)

        // Create texture for entry and exit

        // Draw entry depths

        // Draw exit depths

        // Recover data

        // Process it
    } else {
        // Make texture array big enough for all lights

        // For each light assign index, draw shadows to it

        // 
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
}

void Scene::look(const float deltaX, const float deltaY) {
    camera.look(deltaX, deltaY);
}

void Scene::resizeView(const int width, const int height) {
    camera.resizeView(width, height);
}

mat4 Scene::getView() const {
    return camera.getView();
}

mat4 Scene::getProj() const {
    return camera.getProj();
}

// TODO should return a projection to include all visible (to the light) vertices
mat4 Scene::getProj(const Light *const light) const {
    return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 20.0f);
}

vec3 Scene::getCameraPos() const {
    return camera.getPos();
}

// #include <iostream>

// TODO temporary
mat4 Scene::getLightMatrix() const {
    // std::cout << "lights: " << lights.size() << std::endl;
    const mat4 proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 20.0f);
    // const mat4 proj = glm::perspective(glm::radians(45.0f), 1024.0f / 1024.0f, 0.1f, 100.0f);
    const mat4 view = lights[0]->getView();

    // std::cout << proj[0][0] << std::endl;
    // std::cout << view[0][0] << std::endl << std::endl;

    return proj * view;
}