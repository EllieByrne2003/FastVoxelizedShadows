#include "forest.hpp"

#include <iostream>
#include <cassert>
#include <limits>
#include <memory>
#include <map>

#include <GL/glew.h>
#include <GL/gl.h>

// #define MAX_DEPTH 18

#define MAX_NODES (1024*1024)

Forest::Forest() {
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_TEXTURE_BUFFER, bufferID);
    // glBufferData(GL_TEXTURE_BUFFER, MAX_NODES * sizeof(Node), nullptr, GL_DYNAMIC_DRAW);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_BUFFER, textureID);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RG32UI, bufferID);

    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    glBindTexture(GL_TEXTURE_BUFFER, 0);
}

Forest::~Forest() {
    glDeleteBuffers(1, &bufferID);
    glDeleteTextures(1, &textureID);
}

int Forest::addTree(const SVO &svo) {
    const int startCount = nodes.size();

    int childInd = addChildren(svo);
    // int count    =  0;

    // Node root;
    // root.setChildrenBaseIndex(childInd);
    // for(int i = 0; i < 8; i++) {
    //     const TreeState state = svo.is(i);

    //     if(state == TreeState::DIM) {
    //         root.childSetDim(i);
    //     } else if(state == TreeState::LIT) {
    //         root.childSetLit(i);
    //     } else if(state == TreeState::SPL) {
    //         root.setChildOffset(i, count++);
    //     } else {
    //         assert(false); // Only possible is catastrophic error before
    //     }
    // }

    const int rootInd = addNode(svo, childInd);

    // const int rootInd = nodes.size();
    // nodes.push_back(root);
    const int endCount = nodes.size();
    const int newNodes = endCount - startCount;
    std::cout << "new Nodes: " << newNodes << std::endl;
    std::cout << "new bytes: " << newNodes * sizeof(Node) << std::endl;
    std::cout << "offset:    " << startCount * sizeof(Node) << std::endl;

    glBindBuffer(GL_TEXTURE_BUFFER, bufferID);
    glBufferData(GL_TEXTURE_BUFFER, nodes.size() * sizeof(Node), nodes.data(), GL_STATIC_DRAW);
    // glBufferSubData(GL_TEXTURE_BUFFER, startCount * sizeof(Node), (endCount - startCount) * sizeof(Node), nodes.data() + startCount * sizeof(Node));
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    return rootInd;
}

int Forest::addChildren(const SVO &svo) {
    const std::shared_ptr<SVO> *children = svo.getChildren();

    int childInd = -1;

    std::map<std::shared_ptr<SVO>, int> grandChildrenInd;
    // int grandChildInd[8];

    // Look for grandChildren to add
    for(int i = 0; i < 8; i++) {
        const std::shared_ptr<SVO> &child = children[i];

        if(grandChildrenInd.find(child) != grandChildrenInd.end()) {
            // Alread in there, skip
        } else if(child != nullptr) {
            grandChildrenInd[child] = addChildren(*child);
        }

        // if(child != nullptr) {
        //     grandChildInd[i] = addChildren(*child);
        // } else {
        //     grandChildInd[i] = -1;
        // }
    }

    std::map<std::shared_ptr<SVO>, int> childrenInd;

    // Add children
    for(int i = 0; i < 8; i++) {
        const std::shared_ptr<SVO> &child = children[i];

        if(childrenInd.find(child) != childrenInd.end()) {
            // Skip
        } else if(child != nullptr) {
            const int ind = addNode(*child, grandChildrenInd[child]);
            childrenInd[child] = ind;

            if(childInd == -1) {
                childInd = ind;
            }
        }

        // if(child != nullptr) {
        //     const int ind = addNode(*child, grandChildInd[i]);

        //     if(childInd == -1) {
        //         childInd = ind;
        //     }
        // }
    }

    return childInd;
}

int Forest::addNode(const SVO &svo, const int childInd) {
    const int ind = nodes.size();

    const std::shared_ptr<SVO> *children = svo.getChildren();
    std::map<std::shared_ptr<SVO>, int> offsets;

    int count = 0;

    Node node;
    node.setChildrenBaseIndex(childInd);
    for(int i = 0; i < 8; i++) {
        const TreeState state = svo.is(i);

        if(state == TreeState::DIM) {
            node.childSetDim(i);
        } else if(state == TreeState::LIT) {
            node.childSetLit(i);
        } else if(state == TreeState::SPL) {
            const std::shared_ptr<SVO> &child = children[i];

            if(offsets.find(child) != offsets.end()) {
                node.setChildOffset(i, offsets[child]);
            } else {
                node.setChildOffset(i, count);
                offsets[child] = count;

                count++;
            }
            // node.setChildOffset(i, count++);
        } else {
            assert(false); // Only possible is catastrophic error before
        }
    }

    nodes.push_back(node);

    return ind;
}

bool Forest::equals(const Node &node, const SVO &svo) const {
    // TODO don't implement, not needed or wanted right now

    return true;
}

void Forest::bind(const GLuint voxelsLoc, const GLuint voxelCountLoc, const int slot) const {
    // Bind voxels
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_BUFFER, textureID);
    glUniform1i(voxelsLoc, slot);

    // Bind voxel count
    glUniform1i(voxelCountLoc, nodes.size());
}