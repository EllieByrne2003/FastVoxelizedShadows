#include "svo.hpp"

#include <iostream>
#include <cassert>
#include <memory>

#include "../hierarchy/hierarchy.hpp"

int getIndex(const ivec3 &coords) {
    return (coords.z & 0b1) << 2 | (coords.y & 0b1) << 1 | (coords.x & 0b1);
}

SVO::SVO() : SVO(0) {
}

SVO::SVO(const int height, const TreeState state) : height(height) {
    assert(height >= 0);

    for(int i = 0; i < 8; i++) {
        states[i] = state;
    }
}

SVO::~SVO() {

}

void SVO::split(const int index) {
    assert(height > 0);
    assert(states[index] != SPL);

    children[index] = std::make_shared<SVO>(height - 1, states[index]);
    states[index]   = TreeState::SPL;
}

void SVO::merge(const int index) {
    const TreeState state = states[index];
    
    if(state != TreeState::SPL) {
        return;
    }

    std::shared_ptr<SVO> &child = children[index];

    // Check first can be merged (also checks others with later check for equality)
    if(child->states[0] == SPL) {
        return;
    }

    for(int i = 0; i < 7; i++) {
        if(child->states[i] != child->states[i+1]) {
            return;
        }
    }

    states[index] = child->states[0];
    child         = nullptr;
}

void SVO::compress() {
    // std::cout << "Trying to compress now." << std::endl;

    // Compress children first
    for(int index = 0; index < 8; index++) {
        if(states[index] == SPL) {
            children[index]->compress();
        }
    }

    // Resolve UKN
    bool allLitOrUkn = true;
    for(int index = 0; index < 8; index++) {
        const TreeState state = states[index];
        
        if(state != TreeState::LIT || state != TreeState::UKN) {
            allLitOrUkn = false;
            break;
        }
    }

    if(allLitOrUkn) {
        for(int index = 0; index < 8; index++) {
            states[index] = TreeState::LIT;
        }
    } else {
        for(int index = 0; index < 8; index++) {
            if(states[index] == TreeState::UKN) {
                states[index] = TreeState::DIM;
            }
        }
    }

    // Try to merge them all
    for(int index = 0; index < 8; index++) {
        merge(index);
    }

    // Point from one to next
    for(int index = 0; index < 7; index++) {
        std::shared_ptr<SVO> &a = children[index];
        std::shared_ptr<SVO> &b = children[index + 1];

        if(a == nullptr || b == nullptr) {
            continue;
        }

        if(a == b) {
            continue; // Already pointing to same thing
        }

        if(*a == *b) {
            a = b;    // They're the same, can point to same
        }
    }
}

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
    if(state != TreeState::SPL) {
        return state;
    }

    return children[state]->is(coords);
}

void SVO::inputHierarchy(const Hierarchy &hierarchy, const int baseX, const int baseY, const int baseZ) {
    std::cout << "Called input hierarchy for height: " << height << std::endl;

    for(int y = 0; y < 2; y++) {
        const int realX = baseY + y;

        for(int x = 0; x < 2; x++) {
            const int realY = baseX + x;

            // float entry = hierarchy.getEntry(ivec2(x, y), height);
            // float exit  = hierarchy.getExit( ivec2(x, y), height);

            // for(int z = 0; z < 2; z++) {
            //     const int index = getIndex(ivec3(x, y, z));

            //     const int realZ = parentPos.z + ((z + 1) << height);

            //     std::cout << entry << " " << realZ << " " << exit << std::endl;
            //     if(realZ < entry) {
            //         std::cout << "lit" << std::endl;
            //         states[index] = TreeState::LIT;
            //         // set(ivec3(x, y, z), TreeState::LIT);
            //     } else if(realZ > exit) {
            //         std::cout << "dim" << std::endl;
            //         states[index] = TreeState::DIM;
            //         // set(ivec3(x, y, z), TreeState::DIM);
            //     } else if(height == 0) {
            //         // Just leave it undefined
            //     } else {
            //         split(index); // TODO make new split function

            //         children[index]->inputHierarchy(hierarchy, parentPos + (ivec3(x, y, z) << height));
            //     }
            // }

            float minEntry = hierarchy.getMinEntry(ivec2(x, y), height);
            float maxEntry = hierarchy.getMaxEntry(ivec2(x, y), height);
            float minExit  = hierarchy.getMinExit( ivec2(x, y), height);
            float maxExit  = hierarchy.getMaxExit( ivec2(x, y), height);

            for(int z = 0; z < 2; z++) {
                const int index = getIndex(ivec3(x, y, z));

                const int minZ = baseZ + ((z + 0) << height);
                const int midZ = baseZ + ((z + 0.5) * (1 << height));
                const int maxZ = baseZ + ((z + 1)  << height);
                // const int realZ = parentPos.z + ((z + 1) << height);

                std::cout << minEntry << " " << maxEntry << ", " << minZ << " " << maxZ << ", " << minExit << " " << maxExit << std::endl;
                // if(maxZ < minEntry) {
                //     std::cout << "lit" << std::endl;
                //     states[index] = TreeState::LIT;
                // } else if(minZ >= minExit) {
                //     std::cout << "dim" << std::endl;
                //     states[index] = TreeState::DIM;
                // } else if(minZ < maxEntry || maxZ < maxExit) {
                //     if(height > 0) {
                //         std::cout << "spl" << std::endl;
                //         split(index); // TODO make new split function
                //         children[index]->inputHierarchy(hierarchy, realX << 1, realY << 1, minZ);
                //     } else {
                //         states[index] = TreeState::UKN;
                //         std::cout << "ukn" << std::endl;                        
                //     }
                // } else {
                //     states[index] = TreeState::UKN;
                //     std::cout << "ukn" << std::endl;
                // }
                // } else if(maxZ > maxEntry && minZ < maxExit) {
                //     std::cout << "ukn" << std::endl;
                //     states[index] = TreeState::DIM;
                // } else if(height > 0) {
                //     split(index); // TODO make new split function

                //     children[index]->inputHierarchy(hierarchy, parentPos + (ivec3(x, y, z) << height));
                // }

                if(minZ <= minEntry) {
                    std::cout << "lit" << std::endl;
                    states[index] = TreeState::LIT;
                } else if(minZ > maxEntry) {
                    std::cout << "ukn" << std::endl;
                    // split(index); // TODO make new split function
                    // children[index]->inputHierarchy(hierarchy, realX << 1, realY << 1, minZ);
                } else if(minZ >= maxExit) {
                    std::cout << "dim" << std::endl;
                    states[index] = TreeState::DIM;
                } else if(maxZ < minExit) {
                    std::cout << "ukn" << std::endl;
                } else {
                    if(height > 0) {
                        std::cout << "spl" << std::endl;
                        split(index); // TODO make new split function
                        children[index]->inputHierarchy(hierarchy, realX << 1, realY << 1, minZ);
                    } else {
                        std::cout << "ukn" << std::endl;
                    }
                }
                
                // else if(maxZ >= minExit) {
                //     std::cout << "dim" << std::endl;
                //     states[index] = TreeState::DIM;
                // } else {
                //     if(height > 0) {
                //         std::cout << "spl" << std::endl;
                //         split(index); // TODO make new split function
                //         children[index]->inputHierarchy(hierarchy, realX << 1, realY << 1, minZ);
                //     } else {
                //         std::cout << "ukn" << std::endl;
                //     }
                // }

                // if(realZ < minEntry) { // Lit
                //     std::cout << "lit" << std::endl;
                //     states[index] = TreeState::LIT;
                //     // set(ivec3(x, y, z), TreeState::LIT);
                // } else if(realZ < maxEntry) {
                //     if(height > 0) {
                //         split(index); // TODO make new split function

                //         children[index]->inputHierarchy(hierarchy, parentPos + (ivec3(x, y, z) << height));
                //     }                
                // }else if(realZ > maxExit) { // Dim
                //     std::cout << "dim" << std::endl;
                //     states[index] = TreeState::DIM;
                //     // set(ivec3(x, y, z), TreeState::DIM);
                // } else if(realZ > minExit) { // Unknown, go more fine
                //     if(height > 0) {
                //         split(index); // TODO make new split function

                //         children[index]->inputHierarchy(hierarchy, parentPos + (ivec3(x, y, z) << height));
                //     }
                // } else {
                //     // Just ukn
                //     std::cout << "ukn" << std::endl;
                // }
            }
        }
    }

    compress();
}

void SVO::set(const ivec3 &coords, const TreeState state) {
    const int index = getIndex(coords >> height);

    if(height == 0) {
        states[index] = state;
    } else if(states[index] != state) { // If already state, don't care
        if(states[index] != TreeState::SPL) {
            split(index);
        }

        children[index]->set(coords, state);
        merge(index);
    }
}

void SVO::setBelow(const ivec2 &coords, const int z, const TreeState state) {
    for(int i = 0; i < z; i++) {
        set(ivec3(coords, i), state);
    }
}

void SVO::setAbove(const ivec2 &coords, const int z, const TreeState state) {
   for(int i = z; i < (2 << (height)); i++) {
        set(ivec3(coords, i), state);
    }
}

const std::shared_ptr<SVO> * SVO::getChildren() const {
    return children;
}

bool operator==(const SVO &a, const SVO &b) {
    if(a.height != b.height) {
        return false;
    }

    for(int index = 0; index < 8; index++) {
        const TreeState aState = a.states[index];
        const TreeState bState = b.states[index];

        // One state is solid, simple check
        if(aState != TreeState::SPL || bState != TreeState::SPL) {
            if(aState != bState) {
                return false;
            }
        }

        const std::shared_ptr<SVO> &aChild = a.children[index];
        const std::shared_ptr<SVO> &bChild = b.children[index];

        // Pointer comparison
        if(aChild == bChild) {
            continue;
        }

        // Full child check
        if(aChild != bChild) {
            return false;
        }
    }

    return true;
}

bool operator!=(const SVO &a, const SVO &b) {
    if(a.height != b.height) {
        return true;
    }

    for(int index = 0; index < 8; index++) {
        const TreeState aState = a.states[index];
        const TreeState bState = b.states[index];

        // One state is solid, simple check
        if(aState != TreeState::SPL || bState != TreeState::SPL) {
            if(aState != bState) {
                return true;
            }
        }

        const std::shared_ptr<SVO> &aChild = a.children[index];
        const std::shared_ptr<SVO> &bChild = b.children[index];

        // Pointer comparison
        if(aChild == bChild) {
            continue;
        }

        // Full child check
        if(aChild != bChild) {
            return true;
        }
    }

    return false;
}