#pragma once

#include <memory>
#include <string>

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

    static Texture * readTexture(const std::string &filename);

    void bind(const int slot) const;

    // TODO have some way to retrieve image info 
};