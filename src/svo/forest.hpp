#pragma once

#include <mutex>
#include <vector>

#include <glm/glm.hpp>
using namespace glm;

#include "node.hpp"
#include "svo.hpp"

class Forest {
private:
    std::mutex nodeMutex;
    std::vector<Node> nodes;

    // TOD have the stuff for buffering and binding

protected:

public:
    Forest(); // TODO should be undefined initially
    ~Forest();

    bool isLit(const int rootIndex, const ivec3 &coords) const;
    bool isDim(const int rootIndex, const ivec3 &coords) const;

    void setLit(const int rootIndex, const ivec3 &coords) const;
    void setDim(const int rootIndex, const ivec3 &coords) const;

    void setLitAbove(const int rootIndex, const ivec3 &coords, const int depth) const;
    void setDimAbove(const int rootIndex, const ivec3 &coords, const int depth) const;

    void setLitBelow(const int rootIndex, const ivec3 &coords, const int depth) const;
    void setDimBelow(const int rootIndex, const ivec3 &coords, const int depth) const;

    // TODO should merge svo into nodes
    void addTree(const SVO &svo);


    // // Node functions
    // bool childIsNode(const int index) const;
    // bool childIsLeaf(const int index) const;
    // bool childIsLit(const int index) const;
    // bool childIsDim(const int index) const;

    // int getChildCount() const;
    // int getChildIndex(const int index) const;

    // // Leaf functions

};