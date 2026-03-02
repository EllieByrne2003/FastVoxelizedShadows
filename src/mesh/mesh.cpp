#include "mesh.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

Mesh::Mesh(const std::shared_ptr<const GLuint> &VAO, const std::shared_ptr<const GLuint> &VBO, const std::shared_ptr<const GLuint> &EBO, const int indicesCount) :
    VAO(VAO), VBO(VBO), EBO(EBO), indicesCount(indicesCount)
{

}

Mesh::~Mesh() {

}

void Mesh::draw() const {
    glBindVertexArray(*VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}