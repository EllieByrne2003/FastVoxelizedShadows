#pragma once

#include <vector>

#include <glm/glm.hpp>
using namespace glm;

#include "node.hpp"

class SVO {
private:
    const int height;
    const int size;

    Node node;
    std::vector<SVO> children;

    // When true, cannot alter
    bool compressed = false;

protected:

public:
    SVO(const int height, const int size);
    SVO(const int height, const int size, const Node &node);
    ~SVO();
    
    void splitSelf();
    void mergeSelf();

    bool hasGrandChildren() const;

    bool isLit(const ivec3 &coords) const;
    bool isDim(const ivec3 &coords) const;

    void setLit(const ivec3 &coords);
    void setDim(const ivec3 &coords);

    void setLitAbove(const ivec2 &coords, const int z);
    void setDimAbove(const ivec2 &coords, const int z);

    void setLitBelow(const ivec2 &coords, const int z);
    void setDimBelow(const ivec2 &coords, const int z);

    void compress();

    friend bool operator==(const SVO &a, const SVO &b);
    friend bool operator!=(const SVO &a, const SVO &b);
};