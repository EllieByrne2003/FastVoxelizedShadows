#pragma once

#include <string>

typedef unsigned int GLuint;

// meshes.cpp
int readMesh(const std::string &filename, GLuint &VAO, GLuint &VBO, GLuint &EBO);

// gl.cpp
void deleteVertexArrays(GLuint *array);
void deleteBuffer(GLuint *buffer);