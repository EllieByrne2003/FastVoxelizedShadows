#pragma once

#include <limits>

// TODO could have a directory for stuff like this, structs with no functions
struct Bounds {
    float left   = std::numeric_limits<float>::max();
    float right  = std::numeric_limits<float>::lowest();
    float bottom = std::numeric_limits<float>::max();
    float top    = std::numeric_limits<float>::lowest();
    float back   = std::numeric_limits<float>::max();
    float front  = std::numeric_limits<float>::lowest();
};