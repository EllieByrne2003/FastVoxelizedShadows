#include "node.hpp"

// #define DIM 0b1000
// #define LIT 0b1001
// #define UKN 0b1010

Node::Node(const int state) {
    childrenBaseIndex = -2;

    for(int i = 0; i < 8; i++) {
        childrenMask &= 0b0000 << (i * 4);
        childrenMask |= state  << (i * 4);
    }
}

Node::~Node() {

}

inline int Node::getIndex(const int x, const int y, const int z) {
    return (z & 0b1) << 2 | (y & 0b1) << 1 | (x & 0b1);
}

inline int Node::getIndex(const ivec3 &coords) {
    return getIndex(coords.x, coords.y, coords.z);
}

bool Node::childIsNode(const int index) const {
    return !((childrenMask >> (index * 4 + 3)) & 0b1);
}

bool Node::childIsLeaf(const int index) const {
    return (childrenMask >> (index * 4 + 3)) & 0b1;
}

// Assumes child is not a leaf
bool Node::childIsLit(const int index) const {
    return (childrenMask >> (index * 4)) == NodeState::LIT; 
}

// Assume child is not a leaf
bool Node::childIsDim(const int index) const {
    return (childrenMask >> (index * 4)) == NodeState::DIM;
}
    
// bool Node::childIsUkn(const int index) const {
//     return (childrenMask >> (index * 4)) == NodeState::UKN;
// }

void Node::childSetNode(const int index) {
    childrenMask &= ~(0b1111 << (index * 4));
    childrenMask |=   0b1000 << (index * 4);
}

void Node::childSetLit(const int index) {
    childrenMask &= ~(0b1111         << (index * 4));
    childrenMask |=   NodeState::LIT << (index * 4);
}
    
void Node::childSetDim(const int index) {
    childrenMask &= ~(0b1111         << (index * 4));
    childrenMask |=   NodeState::DIM << (index * 4);
}
    
// void Node::childSetUkn(const int index) {
//     childrenMask &= ~(0b1111         << (index * 4));
//     childrenMask |=   NodeState::UKN << (index * 4);
// }

bool Node::childIsNode(const int x, const int y, const int z) const{
    return childIsNode(getIndex(x, y, z));
}

bool Node::childIsLeaf(const int x, const int y, const int z) const {
    return childIsLeaf(getIndex(x, y, z));
}

bool Node::childIsLit(const int x, const int y, const int z) const {
    return childIsLit(getIndex(x, y, z));
}

bool Node::childIsDim(const int x, const int y, const int z) const {
    return childIsDim(getIndex(x, y, z));
}
    
// bool Node::childIsUkn(const int x, const int y, const int z) const {
//     return childIsUkn(getIndex(x, y, z));
// }

void Node::childSetLit(const int x, const int y, const int z) {
    childSetLit(getIndex(x, y, z));
}

void Node::childSetDim(const int x, const int y, const int z) {
    childSetDim(getIndex(x, y, z));
}

// void Node::childSetUkn(const int x, const int y, const int z) {
//     childSetUkn(getIndex(x, y, z));
// }

int Node::getChildrenBaseIndex() const {
    return childrenBaseIndex;
}

void Node::setChildrenBaseIndex(const int childrenBaseIndex) {
    this->childrenBaseIndex = childrenBaseIndex;
}
    
int Node::getChildOffset(const int index) const {
    return (childrenMask >> (index * 4)) & 0b111;
}

void Node::setChildOffset(const int index, const int offset) {
    childrenMask &= ~(0b1111 << (index * 4)); // Clear bits
    childrenMask |=   offset << (index * 4);  // Set bits
}

void Node::getChildOffset(const int x, const int y, const int z) const {
    getChildOffset(getIndex(x, y, z));
}

void Node::setChildOffset(const int x, const int y, const int z, const int offset) {
    setChildOffset(getIndex(x, y, z), offset);
}

int Node::getChildCount() const {
    unsigned int count = 0;

    for(int i = 0; i < 8; i++) {
        if(childIsNode(i)) {
            count++;
        }
    }

    return count;
}

int Node::getChildIndex(const int index) const {
    const unsigned int childOffset = (childrenMask >> (index * 4)) & 0b111;

    return childrenBaseIndex + childOffset;
}

// bool operator==(const Node &a, const Node &b) {
//     return
// }

// bool operator!=(const Node &a, const Node &b) {

// }