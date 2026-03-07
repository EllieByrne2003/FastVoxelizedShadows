#include "utils.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

void deleteVertexArrays(GLuint *array) {
    glDeleteVertexArrays(1, array);
    delete array;
}

void deleteBuffer(GLuint *buffer) {
    glDeleteBuffers(1, buffer);
    delete buffer;
}

void deleteTexture(GLuint *texture) {
    glDeleteTextures(1, texture);
    delete texture;
}