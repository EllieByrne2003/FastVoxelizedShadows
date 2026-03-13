#pragma once

#include <limits>
#include <vector>
#include <memory>

#include <glm/glm.hpp>
using namespace glm;

#include "../bounds/bounds.hpp"

typedef unsigned int GLuint;

class Mesh {
private:
    const std::shared_ptr<const GLuint> VAO;
    const std::shared_ptr<const GLuint> VBO;
    const std::shared_ptr<const GLuint> EBO;
    const int indicesCount;

    const Bounds bounds;

protected:

public:
    Mesh(const std::shared_ptr<const GLuint> &VAO, const std::shared_ptr<const GLuint> &VBO, const std::shared_ptr<const GLuint> &EBO, const int indicesCount, const Bounds &bounds);
    Mesh(const Mesh &other);
    ~Mesh();

    static Mesh * readMesh(const std::string &filename);

    void draw() const;

    void expandBounds(const mat4 &transform, float &left, float &right, float &bottom, float &up, float &far) const;
};