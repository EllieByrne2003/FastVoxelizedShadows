#include "mesh.hpp"

#include <memory>
#include <mutex>
#include <map>

#include <GL/glew.h>
#include <GL/gl.h>

#include "../utils/utils.hpp"

#define MESH_DIR std::string("res/meshes")

Mesh::Mesh(const std::shared_ptr<const GLuint> &VAO, const std::shared_ptr<const GLuint> &VBO, const std::shared_ptr<const GLuint> &EBO, const int indicesCount, const Bounds &bounds) :
    VAO(VAO), VBO(VBO), EBO(EBO), indicesCount(indicesCount), bounds(bounds)
{

}

Mesh::Mesh(const Mesh &other) : Mesh(other.VAO, other.VBO, other.EBO, other.indicesCount, other.bounds) {

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
        Bounds bounds;
        const int indicesCount = readOBJ(filepath, *vao, *vbo, *ebo, bounds);

        if(indicesCount <= 0) {
            delete vao;
            delete vbo;
            delete ebo;
            
            return nullptr;
        }

        std::shared_ptr<GLuint> VAO(vao, deleteVertexArrays);
        std::shared_ptr<GLuint> VBO(vbo, deleteBuffer);
        std::shared_ptr<GLuint> EBO(ebo, deleteBuffer);

        meshCache.emplace(filename, Mesh(VAO, VBO, EBO, indicesCount, bounds));
        return new Mesh(meshCache.at(filename));
    }
}

void Mesh::draw() const {
    glBindVertexArray(*VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::expandBounds(const mat4 &transform, float &left, float &right, float &bottom, float &up, float &far) const {
    // TODO just have these in bounds
    vec3 corners[8] = {
        glm::vec3(bounds.left,  bounds.bottom, bounds.back),
        glm::vec3(bounds.right, bounds.bottom, bounds.back),
        glm::vec3(bounds.left,  bounds.top,    bounds.back),
        glm::vec3(bounds.right, bounds.top,    bounds.back),
        glm::vec3(bounds.left,  bounds.bottom, bounds.front),
        glm::vec3(bounds.right, bounds.bottom, bounds.front),
        glm::vec3(bounds.left,  bounds.top,    bounds.front),
        glm::vec3(bounds.right, bounds.top,    bounds.front)
    };

    for(const vec3 &pos : corners) {
        const vec3 transformed = transform * vec4(pos, 1.0);

        left   = std::min(left,   transformed.x);
        bottom = std::min(bottom, transformed.y);

        right = std::max(right, transformed.x);
        up    = std::max(up,    transformed.y);
        far   = std::max(far,   transformed.z);
    }


    // const vec3 v1 = transform * vec4(bounds.left, bounds.bottom, bounds.back, 1.0);
    // const vec3 v2 = transform * vec4(bounds.right, bounds.top, bounds.front, 1.0);

    // // const vec3 min = transform * vec4(v1, 1.0);
    // // const vec3 max = transform * vec4(v2, 1.0);

    // left   = std::min(left,   std::min(v1.x, v2.x));
    // bottom = std::min(bottom, std::min(v1.y, v2.y));

    // right = std::max(right, std::max(v1.x, v2.x));
    // up    = std::max(up,    std::max(v1.y, v2.y));
    // far   = std::max(far,   std::max(v1.z, v2.z));

    // // if(min.x < left) {
    // //     left = min.x;
    // // }

    // // if(min.y < bottom) {
    // //     bottom = min.y;
    // // }

    // // // Don't check min.z, no need to look backwards

    // // if(max.x > right) {
    // //     right = max.x;
    // // }

    // // if(max.y > up) {
    // //     up = max.y;
    // // }

    // // if(max.z > far) {
    // //     far = max.z;
    // // }
}