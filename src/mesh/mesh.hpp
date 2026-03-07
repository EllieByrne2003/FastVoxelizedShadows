#pragma once

#include <memory>

typedef unsigned int GLuint;

class Mesh {
private:
    const std::shared_ptr<const GLuint> VAO;
    const std::shared_ptr<const GLuint> VBO;
    const std::shared_ptr<const GLuint> EBO;
    const int indicesCount;

protected:

public:
    Mesh(const std::shared_ptr<const GLuint> &VAO, const std::shared_ptr<const GLuint> &VBO, const std::shared_ptr<const GLuint> &EBO, const int indicesCount);
    Mesh(const Mesh &other);
    ~Mesh();

    static Mesh * readMesh(const std::string &filename);

    void draw() const;
};