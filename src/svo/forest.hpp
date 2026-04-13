#pragma once

#include <mutex>
#include <vector>

#include <glm/glm.hpp>
using namespace glm;

#include "node.hpp"
#include "svo.hpp"

typedef unsigned int GLuint;

class Forest {
private:
    GLuint bufferID;
    GLuint textureID;

    std::mutex nodeMutex;

    std::vector<int>  roots;
    std::vector<Node> nodes;


protected:

public:
    Forest(); // TODO should be undefined initially
    ~Forest();

    // TODO should merge svo into nodes
    int addTree(const SVO &svo);
    int addNode(const SVO &svo, const int childInd);
    int addChildren(const SVO &svo);

    void addChildren(Node &parent, const SVO &svo, const int lastComplete);

    bool equals(const Node &node, const SVO &svo) const;

    void bind( const GLuint voxelsLoc, const GLuint voxelCountLoc, const int slot) const;
};