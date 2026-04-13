#pragma once

#include <vector>

#include <glm/glm.hpp>
using namespace glm;

// #include "node.hpp"

// namespace TreeState {
    enum TreeState {
        LIT = 0b1000,
        DIM = 0b1001,
        UKN = 0b1010,

        IN0 = 0b0000,
        IN1 = 0b0001,
        IN2 = 0b0010,
        IN3 = 0b0011,
        IN4 = 0b0100,
        IN5 = 0b0101,
        IN6 = 0b0110,
        IN7 = 0b0111
    };
// };

class SVO {
private:
    TreeState        states[8];
    std::vector<SVO> children;

    const int height;

    // When true, cannot alter
    bool compressed = false;

protected:

public:
    SVO();
    SVO(const int height, const TreeState state = TreeState::UKN);

    // SVO(const int height, const int size);
    // SVO(const int height, const int size, const Node &node);
    ~SVO();

    void split(const int index);
    void merge(const int index);
    void compress();

    // bool      is(const TreeState state) const;
    // TreeState is() const;

    // void set(const TreeState state);

    bool      is(const int index, const TreeState state) const;
    TreeState is(const int index) const;

    bool      is(const ivec3 &coords, const TreeState state) const;
    TreeState is(const ivec3 &coords) const;

    void set(const ivec3 &coords, const TreeState state);
    void setBelow(const ivec2 &coords, const int z, const TreeState state);
    void setAbove(const ivec2 &coords, const int z, const TreeState state);
    
    // TODO make these only ones
    void set(const ivec3 &coords, const TreeState state, const int size);
    void setBelow(const ivec2 &coords, const int z, const TreeState state, const int size);
    void setAbove(const ivec2 &coords, const int z, const TreeState state, const int size);

    const std::vector<SVO> & getChildren() const;
    int getDistinctChildren() const;
    
    // void splitSelf();
    // void mergeSelf();

    // bool hasGrandChildren() const;

    // bool isLit(const ivec3 &coords) const;
    // bool isDim(const ivec3 &coords) const;

    // void setLit(const ivec3 &coords);
    // void setDim(const ivec3 &coords);

    // void setLitAbove(const ivec2 &coords, const int z);
    // void setDimAbove(const ivec2 &coords, const int z);

    // void setLitBelow(const ivec2 &coords, const int z);
    // void setDimBelow(const ivec2 &coords, const int z);

    // void compress();

    friend bool operator==(const SVO &a, const SVO &b);
    friend bool operator!=(const SVO &a, const SVO &b);
};