#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "scene/scene.hpp"

int main() {
    if(!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow *window = glfwCreateWindow(1920, 1080, "Test", nullptr, nullptr);
    if(!window) {
        std::cerr << "Failed to create GLFWwindow." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        return -1;
    }

    std::string sceneName("test.json");

    std::cout << "loading: " << sceneName << std::endl;

    Scene *scene = Scene::readScene(sceneName);

    if(scene != nullptr) {
        std::cout << "Successfully loaded: " << sceneName << std::endl;
    } else {
        std::cout << "Failed to load: " << sceneName << std::endl;
        return -1;
    }

    do {
        glClearColor(0.3f, 0.0f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        scene->draw();

        glfwPollEvents();
        glfwSwapBuffers(window);
    } while(!(glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE)));


    return 0;
}