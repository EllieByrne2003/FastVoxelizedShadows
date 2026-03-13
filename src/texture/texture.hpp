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

    static Texture * createTexture(const int width, const int height, const int nChannels);

    void bind(const GLuint loc, const int slot) const;
    void bindToFrameBuffer() const;
    void unbindFromFrameBuffer() const;

    // TODO have some way to retrieve image info 
};