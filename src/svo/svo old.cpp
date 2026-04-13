#include "svo old.hpp"

#include <cassert>

SVO::SVO(const int height, const int size) : height(height), size(size) {

}

SVO::SVO(const int height, const int size, const Node &node) : height(height), size(size), node(node) {

}

SVO::~SVO() {

}

void SVO::splitSelf() {
    assert(!compressed);

    for(int i = 0; i < 8; i++) {
        children.emplace_back(height - 1, size / 2, node);

        node.childSetNode(i);
        node.setChildOffset(i, i);
    }
}

void SVO::mergeSelf() {
    assert(!compressed);

    const SVO &first = children[0];

    // Don't merge if grandchildren exist
    if(first.children.size() > 0) {
        return;
    }
 
    // All children must be equal
    for(int i = 1; i < 8; i++) {
        if(first != children[1]) {
            return;
        }
    }

    node = first.node;
    children.clear();
}


bool SVO::hasGrandChildren() const {
    for(const SVO &child : children) {
        if(child.children.size() > 0) {
            return true;
        }
    }

    return false;
}


bool SVO::isLit(const ivec3 &coords) const {
    if(compressed && height == 3) {
        const int z = coords.z & 0b111;

        return children[z].node.isLit(coords.x, coords.y);
    }

    const int childIndex = Node::getIndex(coords >> height);

    if(children.size() == 0) {
        return node.childIsLit(childIndex);
    }

    return children[childIndex].isLit(coords);
}

bool SVO::isDim(const ivec3 &coords) const {
    if(compressed && height == 3) {
        const int z = coords.z & 0b111;

        return children[z].node.isDim(coords.x, coords.y);
    }

    const int childIndex = Node::getIndex(coords >> height);

    if(children.size() == 0) {
        return node.childIsDim(childIndex);
    }

    return children[childIndex].isDim(coords);
}

void SVO::setLit(const ivec3 &coords) {
    assert(!compressed);

    const int childIndex = Node::getIndex(coords >> height);

    // Too deep to have grandchildren
    if(height == 0) {
        node.childSetLit(childIndex);
        return;
    }
    
    // Create child nodes, if needed
    if(!node.childIsNode(childIndex)) {
        splitSelf();
    }

    // Recursive call and merge
    children[childIndex].setLit(coords);
    mergeSelf();
}

void SVO::setDim(const ivec3 &coords) {
    assert(!compressed);

    const int childIndex = Node::getIndex(coords >> height);

    // Too deep to have grandchildren
    if(height == 0) {
        node.childSetDim(childIndex);
        return;
    }
    
    // Create child nodes, if needed
    if(!node.childIsNode(childIndex)) {
        splitSelf();
    }

    // Recursive call and merge
    children[childIndex].setDim(coords);
    mergeSelf();
}

void SVO::setLitAbove(const ivec2 &coords, const int z) {
    assert(!compressed);

    for(int i = z; i < size; i++) {
        setLit(ivec3(coords, i));
    }
}

void SVO::setDimAbove(const ivec2 &coords, const int z) {
    assert(!compressed);

    for(int i = z; i < size; i++) {
        setDim(ivec3(coords, i));
    }
}

void SVO::setLitBelow(const ivec2 &coords, const int z) {
    assert(!compressed);

    for(int i = 0; i < z; i++) {
        setLit(ivec3(coords, i));
    }
}

void SVO::setDimBelow(const ivec2 &coords, const int z) {
    assert(!compressed);

    for(int i = 0; i < z; i++) {
        setDim(ivec3(coords, i));
    }
}

// TODO not using correct resolution of UKN, consult paper (maybe I am already)
void SVO::compress() {
    // Compress bottom layers
    // Each leaf is at a given z and stores 8 voxels in x and y
    if(height == 3) {
        Node leaves[8];

        for(int z = 0; z < 8; z++) {
            Node &leaf = leaves[z];

            for(int x = 0; x < 8 ; x++) {
                for(int y = 0; y < 8; y++) {
                    // Don't need to bother with any UKN, can only be lit or not
                    if(isLit(ivec3(x, y, z))) {
                        leaf.setLit(x, y);
                    }
                }
            }
        }

        children.clear();
        for(int i = 0; i < 8; i++) {
            
            children.emplace_back(leaves[i], height - 1, size / 2);
        }
    } else {
        // Compress recurisively
        for(SVO &child : children) {
            child.compress();
        }

        if(!hasGrandChildren()) {
            bool allLit = true;
            for(int i = 0; i < 8; i++) {
                if(!node.childIsUkn(i)) {
                    if(!node.childIsLit(i)) allLit = false;
                }
            }

            if(allLit) {
                for(int i = 0; i < 8; i++) {
                    node.childSetLit(i);
                }            
            } else {
                for(int i = 0; i < 8; i++) {
                    if(node.childIsUkn(i)) {
                        node.childSetDim(i);
                    }
                }
            }

            mergeSelf();
        } else {
            for(int i = 0; i < 8; i++) {
                if(node.childIsUkn(i)) {
                    node.childSetDim(i);
                }
            }
        }
    }
    
    compressed = true;
}

bool operator==(const SVO &a, const SVO &b) {
    if(a.node != b.node) {
        return false;
    }

    if(a.children.size() != b.children.size()) {
        return false;
    }

    for(int i = 0; i < a.children.size() && i < b.children.size(); i++) {
        if(a.children[i] != b.children[i]) {
            return false;
        }
    }

    return true;
}

bool operator!=(const SVO &a, const SVO &b) {
    if(a.node != b.node) {
        return true;
    }

    if(a.children.size() != b.children.size()) {
        return true;
    }

    for(int i = 0; i < a.children.size() && i < b.children.size(); i++) {
        if(a.children[i] != b.children[i]) {
            return true;
        }
    }

    return false;
}