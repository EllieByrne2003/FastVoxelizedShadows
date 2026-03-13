#pragma once

#include <string>

#include "../bounds/bounds.hpp"

typedef unsigned int GLuint;

// meshes.cpp
int readOBJ(const std::string &filepath, GLuint &VAO, GLuint &VBO, GLuint &EBO, Bounds &bounds);

// textures.cpp
GLuint createTexture(const std::string &path, int &width, int &height, int &nChannels);

// shaders.cpp
GLuint createShader(const std::string &vertexPath, const std::string &fragmentPath);

// gl.cpp
void deleteVertexArrays(GLuint *array);
void deleteBuffer(GLuint *buffer);
void deleteTexture(GLuint *texture);
void deleteShaderProgram(GLuint *shaderProgram);
void deleteFrameBuffer(GLuint *frameBuffer);