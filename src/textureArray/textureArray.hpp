#pragma once

#include <memory>
#include <string>

typedef unsigned int GLuint;

class TextureArray {
private:
    const std::shared_ptr<const GLuint> id;

    const int count;

    const int width;
    const int height;

    const int nChannels;

protected:

public:
    TextureArray(const std::shared_ptr<const GLuint> &id, const int count, const int width, const int height, const int nChannels);
    ~TextureArray();

    static TextureArray * createTexture(const int count, const int width, const int height, const int nChannels);

    void bind(const GLuint loc, const int slot) const;
    void bindToFrameBuffer(const int layer) const;
    void unbindFromFrameBuffer() const;

    // TODO have some way to retrieve image info 
};