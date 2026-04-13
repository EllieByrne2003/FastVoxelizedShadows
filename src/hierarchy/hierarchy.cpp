#include "hierarchy.hpp"

#include <algorithm>

#include <iostream>

int heightToSize(const int height) {
    return 2 << (height);
    // return std::pow(2, height + 1);
}

Hierarchy::Hierarchy(const float *entries, const float *exits, const int height) : height(height), size(heightToSize(height)) {
    this->minEntries.push_back(entries); // TODO make copies for this, this will cause segfault
    this->maxEntries.push_back(entries);

    this->minExits.push_back(exits);
    this->maxExits.push_back(exits);

    std::cout << "starting height: " << height << std::endl; 

    // For entries
    for(int i = 1; i <= height; i++) {
        const float *previousLayer = this->minEntries[i - 1];

        const int currentSize = heightToSize(height - i);
        const int preSize     = currentSize * 2;
        float *currentLayer = new float[currentSize * currentSize];

        // std::cout << "current size for entries: " << currentSize << std::endl;
        // std::cout << "current height: " << height - i << std::endl;

        for(int y = 0; y < currentSize; y++) {
            for(int x = 0; x < currentSize; x++) {
                const float f0 = previousLayer[(y * 2 + 0) * preSize + (x * 2 + 0)];
                const float f1 = previousLayer[(y * 2 + 0) * preSize + (x * 2 + 1)];
                const float f2 = previousLayer[(y * 2 + 1) * preSize + (x * 2 + 0)];
                const float f3 = previousLayer[(y * 2 + 1) * preSize + (x * 2 + 1)];

                if(currentSize == 2) {
                    std::cout << "Min entry: " << std::min(f0, std::min(f1, std::min(f2, f3))) << std::endl;
                }

                currentLayer[y * currentSize + x] = std::min(f0, std::min(f1, std::min(f2, f3)));
            }
        }

        this->minEntries.push_back(currentLayer);
    }

    // For entries
    for(int i = 1; i <= height; i++) {
        const float *previousLayer = this->maxEntries[i - 1];

        const int currentSize = heightToSize(height - i);
        const int preSize     = currentSize * 2;
        float *currentLayer = new float[currentSize * currentSize];

        // std::cout << "current size for entries: " << currentSize << std::endl;
        // std::cout << "current height: " << height - i << std::endl;

        for(int y = 0; y < currentSize; y++) {
            for(int x = 0; x < currentSize; x++) {
                const float f0 = previousLayer[(y * 2 + 0) * preSize + (x * 2 + 0)];
                const float f1 = previousLayer[(y * 2 + 0) * preSize + (x * 2 + 1)];
                const float f2 = previousLayer[(y * 2 + 1) * preSize + (x * 2 + 0)];
                const float f3 = previousLayer[(y * 2 + 1) * preSize + (x * 2 + 1)];

                if(currentSize == 2) {
                    std::cout << "Max entry: " << std::max(f0, std::max(f1, std::max(f2, f3))) << std::endl;
                }

                currentLayer[y * currentSize + x] = std::max(f0, std::max(f1, std::max(f2, f3)));
            }
        }

        this->maxEntries.push_back(currentLayer);
    }

    // For exits
    for(int i = 1; i <= height; i++) {
        const float *previousLayer = this->minExits[i - 1];

        const int currentSize = heightToSize(height - i);
        const int preSize     = currentSize * 2;
        float *currentLayer = new float[currentSize * currentSize];

        // std::cout << "current size for exits: " << currentSize << std::endl;

        for(int y = 0; y < currentSize; y++) {
            for(int x = 0; x < currentSize; x++) {
                const float f0 = previousLayer[(y * 2 + 0) * preSize + (x * 2 + 0)];
                const float f1 = previousLayer[(y * 2 + 0) * preSize + (x * 2 + 1)];
                const float f2 = previousLayer[(y * 2 + 1) * preSize + (x * 2 + 0)];
                const float f3 = previousLayer[(y * 2 + 1) * preSize + (x * 2 + 1)];

                if(currentSize == 2) {
                  std::cout << "Min exit:  " << std::min(f0, std::min(f1, std::min(f2, f3))) << std::endl;
                }

                currentLayer[y * currentSize + x] = std::min(f0, std::min(f1, std::min(f2, f3)));
            }
        }

        this->minExits.push_back(currentLayer);
    }

    // For exits
    for(int i = 1; i <= height; i++) {
        const float *previousLayer = this->maxExits[i - 1];

        const int currentSize = heightToSize(height - i);
        const int preSize     = currentSize * 2;
        float *currentLayer = new float[currentSize * currentSize];

        // std::cout << "current size for exits: " << currentSize << std::endl;

        for(int y = 0; y < currentSize; y++) {
            for(int x = 0; x < currentSize; x++) {
                const float f0 = previousLayer[(y * 2 + 0) * preSize + (x * 2 + 0)];
                const float f1 = previousLayer[(y * 2 + 0) * preSize + (x * 2 + 1)];
                const float f2 = previousLayer[(y * 2 + 1) * preSize + (x * 2 + 0)];
                const float f3 = previousLayer[(y * 2 + 1) * preSize + (x * 2 + 1)];

                if(currentSize == 2) {
                  std::cout << "Max exit:  " << std::max(f0, std::max(f1, std::max(f2, f3))) << std::endl;
                }

                currentLayer[y * currentSize + x] = std::max(f0, std::max(f1, std::max(f2, f3)));
            }
        }

        this->maxExits.push_back(currentLayer);
    }
}

Hierarchy::~Hierarchy() {
    // TODO bring this back, it is a memory leak
    // for(const float *layer : minEntries) {
    //     delete[] layer;
    // }

    // for(const float *layer : maxEntries) {
    //     delete[] layer;
    // }

    // for(const float *layer : minExits) {
    //     delete[] layer;
    // }


    // for(const float *layer : maxExits) {
    //     delete[] layer;
    // }
}

int Hierarchy::getMinEntry(const ivec2 &coords, const int height) const {
    // const int size = heightToSize(height);

    // std::cout << "Returning entry of: " << std::clamp<float>(entries[height][coords.y * size + coords.x], 0.0, 1.0) * size << std::endl;

    return std::clamp<float>(minEntries[height][coords.y * size + coords.x], 0.0, 1.0) * size;
}

int Hierarchy::getMaxEntry(const ivec2 &coords, const int height) const {
    // const int size = heightToSize(height);

    // std::cout << "Returning entry of: " << std::clamp<float>(entries[height][coords.y * size + coords.x], 0.0, 1.0) * size << std::endl;

    return std::clamp<float>(maxEntries[height][coords.y * size + coords.x], 0.0, 1.0) * size;
}

int Hierarchy::getMinExit(const ivec2 &coords, const int height) const {
    // const int size = heightToSize(height);

    // std::cout << "Returning exit of: " << std::clamp<float>(exits[height][coords.y * size + coords.x], 0.0, 1.0) * size << std::endl;

    return std::clamp<float>(minExits[height][coords.y * size + coords.x], 0.0, 1.0) * size;
}

int Hierarchy::getMaxExit(const ivec2 &coords, const int height) const {
    // const int size = heightToSize(height);

    // std::cout << "Returning exit of: " << std::clamp<float>(exits[height][coords.y * size + coords.x], 0.0, 1.0) * size << std::endl;

    return std::clamp<float>(maxExits[height][coords.y * size + coords.x], 0.0, 1.0) * size;
}