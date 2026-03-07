#include "mesh.hpp"

#include <memory>
#include <mutex>
#include <map>

#include <GL/glew.h>
#include <GL/gl.h>

#include "../utils/utils.hpp"

#define MESH_DIR std::string("res/meshes")

Mesh::Mesh(const std::shared_ptr<const GLuint> &VAO, const std::shared_ptr<const GLuint> &VBO, const std::shared_ptr<const GLuint> &EBO, const int indicesCount) :
    VAO(VAO), VBO(VBO), EBO(EBO), indicesCount(indicesCount)
{

}

Mesh::Mesh(const Mesh &other) : Mesh(other.VAO, other.VBO, other.EBO, other.indicesCount) {

}

Mesh::~Mesh() {

}

Mesh * Mesh::readMesh(const std::string &filename) {
    static std::mutex cacheMutex;
    static std::map<std::string, Mesh> meshCache;

    std::unique_lock lock(cacheMutex);

    if(meshCache.find(filename) != meshCache.end()) {
        return new Mesh(meshCache.at(filename));
    } else {
        // TODO should handle other file formats too
        const std::string filepath = MESH_DIR + "/" + filename;

        GLuint *vao = new GLuint, *vbo = new GLuint, *ebo = new GLuint;
        const int indicesCount = readOBJ(filepath, *vao, *vbo, *ebo);

        if(indicesCount <= 0) {
            delete vao;
            delete vbo;
            delete ebo;
            
            return nullptr;
        }

        std::shared_ptr<GLuint> VAO(vao, deleteVertexArrays);
        std::shared_ptr<GLuint> VBO(vbo, deleteBuffer);
        std::shared_ptr<GLuint> EBO(ebo, deleteBuffer);

        meshCache.emplace(filename, Mesh(VAO, VBO, EBO, indicesCount));
        return new Mesh(meshCache.at(filename));
    }
}

void Mesh::draw() const {
    glBindVertexArray(*VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}