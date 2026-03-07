#pragma once

#include <string>

typedef unsigned int GLuint;

// meshes.cpp
int readOBJ(const std::string &filepath, GLuint &VAO, GLuint &VBO, GLuint &EBO);

// textures.cpp
GLuint createTexture(const std::string &path, int &width, int &height, int &nChannels);

// gl.cpp
void deleteVertexArrays(GLuint *array);
void deleteBuffer(GLuint *buffer);
void deleteTexture(GLuint *texture);