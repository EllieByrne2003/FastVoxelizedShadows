#include "svo1.hpp"

#include <iostream>
#include <cassert>

// #define DIM 0b1000
// #define LIT 0b1001
// #define UKN 0b1010

int getIndex(const ivec3 &coords) {
    return (coords.z & 0b1) << 2 | (coords.y & 0b1) << 1 | (coords.x & 0b1);
}

SVO::SVO() : SVO(0) {
    // for(int index = 0; index < 8; index++) {
    //     bool valid = false;

    //     if(states[index] == TreeState::DIM || states[index] == TreeState::LIT || states[index] == TreeState::UKN) {
    //         valid = true;
    //     }

    //     for(int i = IN0; i <= IN7; i++) {
    //         if(states[index] == i) {
    //             valid = true;
    //             break;
    //         }
    //     }
    //     if(!valid) {
    //         std::cout << "state: " << states[index] << std::endl;
    //         assert(false);
    //     }
    // }
}

SVO::SVO(const int height, const TreeState state) : height(height) {
    // this->state = state;
    assert(height >= 0);

    for(int i = 0; i < 8; i++) {
        states[i] = state;
    }

    // for(int index = 0; index < 8; index++) {
    //     bool valid = false;

    //     if(states[index] == TreeState::DIM || states[index] == TreeState::LIT || states[index] == TreeState::UKN) {
    //         valid = true;
    //     }

    //     for(int i = IN0; i <= IN7; i++) {
    //         if(states[index] == i) {
    //             valid = true;
    //             break;
    //         }
    //     }
    //     if(!valid) {
    //         std::cout << "state: " << states[index] << std::endl;
    //         assert(false);
    //     }
    // }
}

// SVO::SVO(const int height, const int size) : height(height), size(size) {

// }

// SVO::SVO(const int height, const int size, const Node &node) : height(height), size(size), node(node) {

// }

SVO::~SVO() {

}

void SVO::split(const int index) {
    assert(height > 0);
    assert(states[index] == DIM || states[index] == LIT || states[index] == UKN);
    assert(children.size() < 8);

    children.emplace_back(height - 1, states[index]);
    states[index] = static_cast<TreeState>(IN0 + (children.size() - 1));

    // bool valid = false;
    // for(int i = IN0; i <= IN7; i++) {
    //     if(states[index] == i) {
    //         valid = true;
    //         break;
    //     }
    // }
    // if(!valid) {
    //     std::cout << "state: " << states[index] << std::endl;
    //     assert(false);
    // }
}

// void SVO::split() {
//     if(height == 0) return;

//     children.clear(); // Shouldn't be called if not clear but won't hurt

//     for(int i = 0; i < 8; i++) {
//         children.emplace_back(height - 1, state);
//     }

//     state = SPL;
// }

void SVO::merge(const int index) {
    const TreeState state = states[index];
    
    if(state == DIM || state == LIT || state == UKN) {
        return;
    }

    SVO &child = children[states[index]];

    // Check first can be merged (also checks others with later check for equality)
    if(child.states[0] != DIM || child.states[0] != LIT || child.states[0] != UKN) {
        return;
    }

    for(int i = 0; i < 7; i++) {
        if(child.states[i] != child.states[i+1]) {
            return;
        }
    }

    states[index] = child.states[0];
    child.children.clear();

    // if(index != children.size() - 1) {
    //     std::cout << "Merging not the last one" << std::endl; 
    // }
}

void SVO::compress() {
    std::cout << "Trying to compress now." << std::endl;

    // Compress children
    for(SVO &child : children) {
        child.compress();
    }

    // Resolve unknowns
    bool allLitOrUkn = true;
    for(int index = 0; index < 8; index++) {
        if(states[index] != LIT && states[index] != UKN) {
            allLitOrUkn = false;
            break;
        }
    }

    if(allLitOrUkn) {
        for(int index = 0; index < 8; index++) {
            states[index] = LIT;
        }
    } else {
        for(int index = 0; index < 8; index++) {
            if(states[index] == UKN) {
                states[index] = DIM;
            }
        }
    }

    // Merge all children
    for(int index = 0; index < 8; index++) {
        merge(index);
    }

    // // Check equalities
    // for(int i = 0; i < 8; i++) {
    //     if(states[i] == DIM || states[i] == LIT){
    //         continue;
    //     }

    //     for(int j = 0; j < i; j++) {
    //         if(children[states[i]] == children[states[j]]) {
    //             states[i] = states[j]; // Point to previous nodes
    //         }
    //     }
    // }

    // // Trim children
    // int count = 0;
    // for(int i = 0; i < 8; i++) {
    //     if(states[i] == DIM || states[i] == LIT){
    //         continue;
    //     }

    //     count = std::max(count, int(states[i]));
    // }

    // children.resize(count);
}

// void SVO::compress() {
//     // Compress children and try to merge self
//     for(SVO &child : children) {
//         child.compress();
//     }

//     merge();

//     if(children.size() < 8) return; // Compress needs children

//     bool allLitOrUkn = true;

//     for(const SVO &child : children) {
//         if(child.is(DIM) || child.is(SPL)) {
//             allLitOrUkn = false;
//         }
//     }

//     if(allLitOrUkn) {
//         for(SVO &child : children) {
//             child.set(LIT);
//         }
//     } else {
//         for(SVO &child : children) {
//             if(!child.is(LIT) && !child.is(SPL)) {
//                 child.set(DIM);
//             }
//         }
//     }
// }

// bool SVO::is(const TreeState state) const {
//     return this->state == state;
// }

// TreeState SVO::is() const {
//     return state;
// }

// void SVO::set(const TreeState state) {
//     this->state = state;
// }

bool SVO::is(const int index, const TreeState state) const {
    return is(index) == state;
}

TreeState SVO::is(const int index) const {
    return states[index];
}

bool SVO::is(const ivec3 &coords, const TreeState state) const {
    return is(coords) == state;
}

TreeState SVO::is(const ivec3 &coords) const {
    const int index = getIndex(coords >> height);

    const TreeState state = states[index];
    if(state == DIM || state == LIT || state == UKN) {
        return state;
    }

    return children[state].is(coords);
}

// TreeState SVO::is(const ivec3 &coords) const {
//     if(height == 0 || state != SPL || children.size() < 8) return state;

//     const int index = getIndex(coords >> height);
//     return children[index].is(coords);
// }

void SVO::set(const ivec3 &coords, const TreeState state) {
    const int index = getIndex(coords >> height);

    if(height == 0) {
        states[index] = state;
    } else {
        if(states[index] == DIM || states[index] == LIT || states[index] == UKN) {
            split(index);
        } 

        children[states[index]].set(coords, state);
        merge(index);
    }
}

// void SVO::set(const ivec3 &coords, const TreeState state) {
//     if(this->state != SPL || children.size() < 8) {
//         split();
//     }

//     const int index = getIndex(coords >> height);
//     children[index].set(coords, state);
// }

void SVO::setBelow(const ivec2 &coords, const int z, const TreeState state) {
    for(int i = 0; i < z; i++) {
        set(ivec3(coords, i), state);
    }
}

// void SVO::setBelow(const ivec2 &coords, const int z, const TreeState state) {
//     if(this->state == state) return;

//     for(int i = 0; i < z; i++) {
//         set(ivec3(coords, i), state);
//     }

//     merge();
// }

void SVO::setAbove(const ivec2 &coords, const int z, const TreeState state) {
    // std::cout << "z: " << z << " max: " << (2 << (height)) << std::endl;
   for(int i = z; i < (2 << (height)); i++) {
        set(ivec3(coords, i), state);
    }
}

// void SVO::setAbove(const ivec2 &coords, const int z, const TreeState state) {
//     if(this->state == state || height == 0) return;

//     for(int i = z; i > (2 << (height)); i--) {
//         set(ivec3(coords, i), state);
//     }

//     merge();
// }

// void SVO::splitSelf() {
//     assert(!compressed);

//     for(int i = 0; i < 8; i++) {
//         children.emplace_back(height - 1, size / 2, node);

//         node.childSetNode(i);
//         node.setChildOffset(i, i);
//     }
// }

// void SVO::mergeSelf() {
//     assert(!compressed);

//     const SVO &first = children[0];

//     // Don't merge if grandchildren exist
//     if(first.children.size() > 0) {
//         return;
//     }
 
//     // All children must be equal
//     for(int i = 1; i < 8; i++) {
//         if(first != children[1]) {
//             return;
//         }
//     }

//     node = first.node;
//     children.clear();
// }


// bool SVO::hasGrandChildren() const {
//     for(const SVO &child : children) {
//         if(child.children.size() > 0) {
//             return true;
//         }
//     }

//     return false;
// }


// bool SVO::isLit(const ivec3 &coords) const {
//     if(compressed && height == 3) {
//         const int z = coords.z & 0b111;

//         return children[z].node.isLit(coords.x, coords.y);
//     }

//     const int childIndex = Node::getIndex(coords >> height);

//     if(children.size() == 0) {
//         return node.childIsLit(childIndex);
//     }

//     return children[childIndex].isLit(coords);
// }

// bool SVO::isDim(const ivec3 &coords) const {
//     if(compressed && height == 3) {
//         const int z = coords.z & 0b111;

//         return children[z].node.isDim(coords.x, coords.y);
//     }

//     const int childIndex = Node::getIndex(coords >> height);

//     if(children.size() == 0) {
//         return node.childIsDim(childIndex);
//     }

//     return children[childIndex].isDim(coords);
// }

// void SVO::setLit(const ivec3 &coords) {
//     assert(!compressed);

//     const int childIndex = Node::getIndex(coords >> height);

//     // Too deep to have grandchildren
//     if(height == 0) {
//         node.childSetLit(childIndex);
//         return;
//     }
    
//     // Create child nodes, if needed
//     if(!node.childIsNode(childIndex)) {
//         splitSelf();
//     }

//     // Recursive call and merge
//     children[childIndex].setLit(coords);
//     mergeSelf();
// }

// void SVO::setDim(const ivec3 &coords) {
//     assert(!compressed);

//     const int childIndex = Node::getIndex(coords >> height);

//     // Too deep to have grandchildren
//     if(height == 0) {
//         node.childSetDim(childIndex);
//         return;
//     }
    
//     // Create child nodes, if needed
//     if(!node.childIsNode(childIndex)) {
//         splitSelf();
//     }

//     // Recursive call and merge
//     children[childIndex].setDim(coords);
//     mergeSelf();
// }

// void SVO::setLitAbove(const ivec2 &coords, const int z) {
//     assert(!compressed);

//     for(int i = z; i < size; i++) {
//         setLit(ivec3(coords, i));
//     }
// }

// void SVO::setDimAbove(const ivec2 &coords, const int z) {
//     assert(!compressed);

//     for(int i = z; i < size; i++) {
//         setDim(ivec3(coords, i));
//     }
// }

// void SVO::setLitBelow(const ivec2 &coords, const int z) {
//     assert(!compressed);

//     for(int i = 0; i < z; i++) {
//         setLit(ivec3(coords, i));
//     }
// }

// void SVO::setDimBelow(const ivec2 &coords, const int z) {
//     assert(!compressed);

//     for(int i = 0; i < z; i++) {
//         setDim(ivec3(coords, i));
//     }
// }

// // TODO not using correct resolution of UKN, consult paper (maybe I am already)
// void SVO::compress() {
//     // Compress bottom layers
//     // Each leaf is at a given z and stores 8 voxels in x and y
//     if(height == 3) {
//         Node leaves[8];

//         for(int z = 0; z < 8; z++) {
//             Node &leaf = leaves[z];

//             for(int x = 0; x < 8 ; x++) {
//                 for(int y = 0; y < 8; y++) {
//                     // Don't need to bother with any UKN, can only be lit or not
//                     if(isLit(ivec3(x, y, z))) {
//                         leaf.setLit(x, y);
//                     }
//                 }
//             }
//         }

//         children.clear();
//         for(int i = 0; i < 8; i++) {
            
//             children.emplace_back(leaves[i], height - 1, size / 2);
//         }
//     } else {
//         // Compress recurisively
//         for(SVO &child : children) {
//             child.compress();
//         }

//         if(!hasGrandChildren()) {
//             bool allLit = true;
//             for(int i = 0; i < 8; i++) {
//                 if(!node.childIsUkn(i)) {
//                     if(!node.childIsLit(i)) allLit = false;
//                 }
//             }

//             if(allLit) {
//                 for(int i = 0; i < 8; i++) {
//                     node.childSetLit(i);
//                 }            
//             } else {
//                 for(int i = 0; i < 8; i++) {
//                     if(node.childIsUkn(i)) {
//                         node.childSetDim(i);
//                     }
//                 }
//             }

//             mergeSelf();
//         } else {
//             for(int i = 0; i < 8; i++) {
//                 if(node.childIsUkn(i)) {
//                     node.childSetDim(i);
//                 }
//             }
//         }
//     }
    
//     compressed = true;
// }

// int SVO::getDistinctChildren() const {
//     assert(children.size() == 8);

//     int unique = 8;
//     for(int i = 0; i < 8; i++) {
//         for(int j = i + 1; j < 8; j++) {
//             if(children[i] == children[j]) {
//                 unique--;
//                 break;
//             }
//         }
//     }

//     return unique;
// }

const std::vector<SVO> & SVO::getChildren() const {
    return children;
}

bool operator==(const SVO &a, const SVO &b) {
    if(a.height != b.height) {
        return false;
    }

    for(int i = 0; i < 8; i++) {
        const TreeState aState = a.states[i];
        const TreeState bState = b.states[i];

        // Not yet true, check next
        if(aState == bState) {
            continue;
        }

        // If aState isn't a child, they're different
        if(aState == DIM || aState == LIT ||  aState == UKN) {
            return false;
        }

        // If bState isn't a child, they're different
        if(bState == DIM || bState == LIT ||  bState == UKN) {
            return false;
        }

        // If both have children, check children equality, 
        if(a.children[aState] != b.children[bState]) {
            return false;
        }
    }

    std::cout << "A and B are the same" << std::endl;

    return true;
}

bool operator!=(const SVO &a, const SVO &b) {
    for(int i = 0; i < 8; i++) {
        const TreeState aState = a.states[i];
        const TreeState bState = b.states[i];

        // Not yet false, try next
        if(aState == bState) {
            continue;
        }

        // If aState isn't a child, they're different
        if(aState == DIM || aState == LIT ||  aState == UKN) {
            return true;
        }

        // If bState isn't a child, they're different
        if(bState == DIM || bState == LIT ||  bState == UKN) {
            return true;
        }

        // If both have children, check children equality
        if(a.children[aState] != b.children[bState]) {
            return true;
        }

        // if(aState == DIM || aState == LIT ||  aState == UKN) {
        //     if(bState == DIM || bState == LIT ||  bState == UKN) { // both a and b are solid
        //         if(aState != bState) {
        //             return true;
        //         } else {
        //             continue; // Same, skip
        //         }
        //     } else { // a is solid but b isn't
        //         return true;
        //     }
        // } else if(bState == DIM || bState == LIT ||  bState == UKN) { // a isn't solid but b is
        //     return true;
        // } else if(a.children[aState] != b.children[bState]) {
        //     return true;
        // }
    }

    return false;
}

// bool operator==(const SVO &a, const SVO &b) {
//     if(a.state != b.state) {
//         return false;
//     }

//     // if(a.children.size() != b.children.size()) {
//     //     return false;
//     // }

//     for(int i = 0; i < a.children.size() && i < b.children.size(); i++) {
//         if(a.children[i] != b.children[i]) {
//             return false;
//         }
//     }

//     return true;
// }

// bool operator!=(const SVO &a, const SVO &b) {
//     if(a.state != b.state) {
//         return true;
//     }

//     // if(a.children.size() != b.children.size()) {
//     //     return true;
//     // }

//     for(int i = 0; i < a.children.size() && i < b.children.size(); i++) {
//         if(a.children[i] != b.children[i]) {
//             return true;
//         }
//     }

//     return false;
// }