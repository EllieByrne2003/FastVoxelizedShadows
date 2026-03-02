#pragma once

#include <memory>

typedef unsigned int GLuint;

class Texture {
private:
    const std::shared_ptr<const GLuint> id;

    const int width;
    const int height;

    const int nChannels;

protected:

public:
    Texture(const std::shared_ptr<const GLuint> &id, const int width, const int height, const int nChannels);
    ~Texture();

    void bind(const int slot) const;

    // TODO have some way to retrieve image info 
};