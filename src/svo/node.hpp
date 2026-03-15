#pragma once

#include <glm/glm.hpp>
using namespace glm;

#define DIM 0b1000
#define LIT 0b1001
#define UKN 0b1010

// TODO note that all functions assume acceptable values are given
class Node {
private:
    union {
        struct {
            int          childrenBaseIndex;
            unsigned int childrenMask;
        };

        // For lowest 3 levels
        unsigned long shadowBits;
    };

protected:

public:
    Node(const int state = UKN);
    ~Node();

    // Convert to index
    static inline int getIndex(const int x, const int y, const int z);
    static inline int getIndex(const ivec3 &coords);

    // Node functions (being treated as a node)
    bool childIsNode(const int index) const;
    bool childIsLeaf(const int index) const;
    bool childIsLit(const int index) const;
    bool childIsDim(const int index) const;
    bool childIsUkn(const int index) const;

    void childSetNode(const int index);
    void childSetLit(const int index);
    void childSetDim(const int index);
    void childSetUkn(const int index);

    bool childIsNode(const int x, const int y, const int z) const;
    bool childIsLeaf(const int x, const int y, const int z) const;
    bool childIsLit(const int x, const int y, const int z) const;
    bool childIsDim(const int x, const int y, const int z) const;
    bool childIsUkn(const int x, const int y, const int z) const;

    void childSetLit(const int x, const int y, const int z);
    void childSetDim(const int x, const int y, const int z);
    void childSetUkn(const int x, const int y, const int z);

    int  getChildrenBaseIndex() const;
    void setChildrenBaseIndex(const int childrenBaseIndex);
    
    int  getChildOffset(const int index) const;
    void setChildOffset(const int index, const int offset);

    void getChildOffset(const int x, const int y, const int z) const;
    void setChildOffset(const int x, const int y, const int z, const int offset);

    int getChildCount() const;
    int getChildIndex(const int index) const;

    // Leaf functions (being treated as a leaf)
    bool isLit(const int x, const int y) const;
    bool isDim(const int x, const int y) const;
    bool isUkn(const int x, const int y) const;

    void setLit(const int x, const int y);
    void setDim(const int x, const int y);
    bool setUkn(const int x, const int y);

    // TODO implement
    // bool isAllLit() const;
    // bool isAllDim() const;

    friend bool operator==(const Node &a, const Node &b);
    friend bool operator!=(const Node &a, const Node &b);
};