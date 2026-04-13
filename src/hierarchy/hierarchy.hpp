#pragma once

#include <vector>

#include <glm/glm.hpp>
using namespace glm;

class Hierarchy {
private:
    std::vector<const float *> minEntries;
    std::vector<const float *> maxEntries;
    
    std::vector<const float *> minExits;
    std::vector<const float *> maxExits;

    const int height;
    const int size;

protected:

public:
    Hierarchy(const float *entries, const float *exits, const int height);
    ~Hierarchy();

    int getMinEntry(const ivec2 &coords, const int height) const;
    int getMaxEntry(const ivec2 &coords, const int height) const;

    int getMinExit(const ivec2 &coords, const int height) const;
    int getMaxExit(const ivec2 &coords, const int height) const;
};