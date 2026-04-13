#include "scene.hpp"

#include <fstream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "../svo/svo.hpp"
#include "../light/light.hpp"
#include "../models/models.hpp"
#include "../texture/texture.hpp"
#include "../renderer/renderer.hpp"
#include "../hierarchy/hierarchy.hpp"
#include "../textureArray/textureArray.hpp"

#include <iostream>

#define SCENE_DIR std::string("res/scenes")

#define HEIGHT 10
#define SIZE   2048

Scene::Scene(const std::string &name, const std::string &description) {
    this->name = name;
    this->description = description;

    glGenBuffers(1, &lightsSSBO);

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Create a depth renderbuffer
    GLuint depthRbo;
    glGenRenderbuffers(1, &depthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, SIZE, SIZE); // TODO move elsewhere, a define maybe
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRbo);

    // Optional: if you also need stencil, use GL_DEPTH24_STENCIL8 and attach to GL_DEPTH_STENCIL_ATTACHMENT

    // Now check completeness – must be COMPLETE
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer still incomplete after attaching depth." << std::endl;
        // handle error
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Scene::~Scene() {
    for(Model *model : models) {
        delete model;
    }

    // for(Light *light : lights) {
    //     delete light;
    // }

    glDeleteFramebuffers(1, &FBO);
    glDeleteBuffers(1, &lightsSSBO);
    
    if(depthMaps != nullptr) {
        delete depthMaps;
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    camera.setViewPort(); // Sets the viewport to the size of the scenes camera

    for(Model *model : models) {
        model->draw(this);
    }
}

void Scene::drawDepths(const Light &light, const bool drawEntry) {
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);


    glEnable(GL_CULL_FACE);
    if(drawEntry) {
        glCullFace(GL_BACK);
    } else {
        glCullFace(GL_FRONT);
    }

    for(Model *model : models) {
        model->drawDepths(this, light, drawEntry); // TODO don't need to pass drawEntry anymore
    }

    // glCullFace(GL_BACK);
    // glDisable(GL_CULL_FACE);
}

void Scene::setupLights(const bool voxelize) {
    // TODO do this, for now just bodge it
    // const int size = 4096;


    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);

    glViewport(0, 0, SIZE, SIZE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // TODO error needs handling
        std::cout << "Failed to make framebuffer." << std::endl;
        // return;
    }

    float *test0 = new float[SIZE * SIZE];
    float *test1 = new float[SIZE * SIZE];
    
    std::fill_n(test0, SIZE * SIZE, 0.25);
    std::fill_n(test1, SIZE * SIZE, 0.9);

    for(int i = 0; i < lights.size(); i++) {
        Light &light = lights[i];
        light.setLightSpaceMatrix(getProj(light));

        glClear(GL_DEPTH_BUFFER_BIT);
        float *entries = new float[SIZE * SIZE];
        this->drawDepths(light, true);
        glFinish();
        glReadPixels(0, 0, SIZE, SIZE, GL_DEPTH_COMPONENT, GL_FLOAT, entries);

        glClear(GL_DEPTH_BUFFER_BIT);
        float *exits = new float[SIZE * SIZE];
        this->drawDepths(light, false);
        glFinish();
        glReadPixels(0, 0, SIZE, SIZE, GL_DEPTH_COMPONENT, GL_FLOAT, exits);

        std::cout << "Read pixels" << std::endl;


        Hierarchy hierarchy(entries, exits, HEIGHT);


        SVO svo(HEIGHT);
        for(int y = 0; y < SIZE; y++) {
            for(int x = 0; x < SIZE; x++) {


                // std::cout << "entry: " << entries[y * 8*1024 + x] << std::endl;
                // std::cout << "exit:  " <<   exits[y * 8*1024 + x] << std::endl;
                // svo.setBelow(ivec2(x, y), entries[y * 8*1024 + x] * 8*1024,TreeState::LIT);
                // svo.setAbove(ivec2(x, y),   exits[y * 8*1024 + x] * 8*1024,TreeState::DIM);
            
                // svo.setBelow(ivec2(x, y), size / 4, TreeState::LIT);
                // svo.setAbove(ivec2(x, y), size / 2, TreeState::DIM);


                // std::cout << "entry: " << int(entries[y * size + x] * size) << std::endl;
                // std::cout << "exit:  " << int(  exits[y * size + x] * size) << std::endl;
                // svo.setBelow(ivec2(x, y), (SIZE * 3) / 4,  TreeState::LIT);
                svo.setBelow(ivec2(x, y), std::clamp<float>(entries[y * SIZE + x], 0.0, 1.0) * SIZE,  TreeState::LIT);
                svo.set(     ivec3(x, y,  std::clamp<float>(entries[y * SIZE + x], 0.0, 1.0) * SIZE), TreeState::LIT);

                svo.setAbove(ivec2(x, y), std::clamp(int(  exits[y * SIZE + x] * SIZE), 0, SIZE), TreeState::DIM);
                // svo.setAbove(ivec2(x, y), SIZE / 4, TreeState::DIM);
            }
        }

        // Hierarchy hierarchy(entries, exits, HEIGHT);
        // // Hierarchy hierarchy(test0, test1, HEIGHT);

        // std::cout << "Hierarchy made" << std::endl;
        
        // SVO svo(HEIGHT);
        // svo.inputHierarchy(hierarchy);
        // svo2.compress();

        std::cout << "Input hierarchy" << std::endl;


        // std::cout << "SVO made" << std::endl;

        svo.compress();
        std::cout << "SVO compressed" << std::endl;

        // if(svo == svo2) {
        //     std::cout << "They are the same" << std::endl;
        // } else {
        //     std::cout << "They are not the same" << std::endl;
        // }

        const int root = forest.addTree(svo);
        light.setVoxelRootIndex(root);

        std::cout << "Tree " << i << " made." << std::endl;

        // delete[] entries;
        // delete[] exits;
    }

    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // This is for typical shadow maps
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glViewport(0, 0, 8*1024, 8*1024);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // TODO error needs handling
    }

    for(int i = 0; i < lights.size(); i++) {
        Light &light = lights[i];
        light.setLightSpaceMatrix(getProj(light));

        depthMaps->bindToFrameBuffer(i);
        glClear(GL_DEPTH_BUFFER_BIT);

        this->drawDepths(light, true);

        if(voxelize) {
            // TODO implement
        } else {

        }
    }

    glViewport(0, 0, 1980, 1080);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::addModel(Model *model) {
    models.push_back(model);
}

void Scene::addLight(Light *light) {
    light->setDepthMapIndex(lights.size());
    lights.push_back(*light);

    delete light;

    // Make new texture array for lights
    if(depthMaps != nullptr) {
        delete depthMaps;
    }

    depthMaps = TextureArray::createTexture(lights.size(), 8*1024, 8*1024, 1);
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


mat4 Scene::getProj(const Light &light) const {
    Bounds bounds;

    mat4 tempProj;
    if(light.isDirectional()) {
        tempProj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f); // Just to do calculations
    } else{
        tempProj = glm::perspective(glm::radians(30.0f), 1.0f, 0.0f, 1.0f);
    }

    // We need to iterate through all model
    for(const Model *const model : models) {
        model->expandBounds(bounds, light.getView(), tempProj);
    }
    

    if(light.isDirectional()) {
        
        return glm::ortho(bounds.left, bounds.right, bounds.bottom, bounds.top, 0.0f, bounds.front);
    } else {
        return glm::perspective(glm::radians(light.getConeAngle()), 1.0f, bounds.back, std::min(bounds.front, light.getIntensity()));
        // return glm::perspective(glm::radians(light.getConeAngle()), 1.0f, 0.1f, light.getIntensity());
    }
}

vec3 Scene::getCameraPos() const {
    return camera.getPos();
}

// #include <iostream>

// TODO temporary
mat4 Scene::getLightMatrix() const {
    // std::cout << "lights: " << lights.size() << std::endl;
    // const mat4 proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 20.0f);

    const mat4 proj = getProj(lights[0]);

    // const mat4 proj = glm::perspective(glm::radians(45.0f), 1024.0f / 1024.0f, 0.1f, 100.0f);
    const mat4 view = lights[0].getView();

    // std::cout << proj[0][0] << std::endl;
    // std::cout << view[0][0] << std::endl << std::endl;

    return proj * view;
}

void Scene::bindLights(const GLuint lightCountLoc, const int lightIndex) const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, lights.size() * sizeof(Light), lights.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, lightIndex, lightsSSBO); // TODO let shaders choose own buffer

    glUniform1i(lightCountLoc, lights.size());
}

void Scene::bindVoxels(const GLuint voxelsLoc, const GLuint voxelCountLoc, const int slot) const {
    forest.bind(voxelsLoc, voxelCountLoc, slot);
}

void Scene::bindDepthMaps(const GLuint depthmapsLoc, const int slot) const {
    if(depthMaps != nullptr) {
        depthMaps->bind(depthmapsLoc, slot);
    }
}