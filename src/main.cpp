#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
using namespace glm;

#include "scene/scene.hpp"
#include "renderer/renderer.hpp"

void processKey(Scene *scene, GLFWwindow *window);
void processCursorPos(Scene *scene, const double xPos, const double yPos);

void GLAPIENTRY OpenGLDebugCallback(
	GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, 
	const GLchar* message, const void* userParam)
{
	// Print the error message
  std::cerr << "OpenGL Debug Message:\n";
  std::cerr << "Source  : " << source << "\n";
  std::cerr << "Type    : " << type << "\n";
  std::cerr << "ID      : " << id << "\n";
  std::cerr << "Severity: " << severity << "\n";
  std::cerr << "Message : " << message << "\n\n"; 
}


int main() {
    if(!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow *window = glfwCreateWindow(1024, 1024, "Test", nullptr, nullptr);
    if(!window) {
        std::cerr << "Failed to create GLFWwindow." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        return -1;
    }

	// Enable OpenGL debug output
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // To make the callback synchronous

	// Register the debug callback
	glDebugMessageCallback(OpenGLDebugCallback, nullptr);

    std::string sceneName("test.json");
    std::cout << "loading: " << sceneName << std::endl;
    Scene *scene = Scene::readScene(sceneName);

    if(scene != nullptr) {
        std::cout << "Successfully loaded: " << sceneName << std::endl;
    } else {
        std::cout << "Failed to load: " << sceneName << std::endl;
        return -1;
    }

    // Max sizes for viewports and textures
    GLint maxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    GLint maxViewportSize[] = {0, 0};
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxViewportSize);

    GLint maxBufferSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxBufferSize);

    std::cout << "Max texture size:  " << maxTextureSize     << "*" << maxTextureSize     << std::endl;
    std::cout << "Max viewport size: " << maxViewportSize[0] << "*" << maxViewportSize[1] << std::endl;
    std::cout << "Max buffer size:   " << maxBufferSize << "*" << maxBufferSize << std::endl;

    glEnable(GL_DEPTH_TEST);
    scene->setupLights(false); // TODO should be true, this should be called each frame

    const mat4 projection = glm::perspective(glm::radians(45.0f), 1980.0f / 1080.0f, 0.1f, 100.0f);
    // Renderer::getInstance().setProj(projection);
    
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    do {
        glClearColor(0.3f, 0.0f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get movement
        processKey(scene, window);

        // Get mouse location and process it
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        processCursorPos(scene, xPos, yPos);

        // Draw scene
        Renderer::getInstance().setProj(projection);
        scene->resizeView(1980, 1080);
        scene->draw();

        glfwPollEvents();
        glfwSwapBuffers(window);
    } while(!(glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE)));


    return 0;
}

void processKey(Scene *scene, GLFWwindow *window) {
    static double lastTime = glfwGetTime();

    const double currentTime = glfwGetTime();
    const double deltaTime   = currentTime - lastTime;
    lastTime = currentTime;

    if(glfwGetKey(window, GLFW_KEY_W))          scene->move(FORWARD,  deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S))          scene->move(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A))          scene->move(LEFT,     deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D))          scene->move(RIGHT,    deltaTime);
    if(glfwGetKey(window, GLFW_KEY_SPACE))      scene->move(UP,       deltaTime);
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) scene->move(DOWN,     deltaTime);

    lastTime = currentTime;
}

void processCursorPos(Scene *scene, const double xPos, const double yPos) {
    static double lastXPos = xPos;
    static double lastYPos = yPos;

    const double deltaX = lastXPos - xPos;
    const double deltaY = lastYPos - yPos;

    scene->look((float) deltaX, (float) deltaY);

    lastXPos = xPos;
    lastYPos = yPos;
}