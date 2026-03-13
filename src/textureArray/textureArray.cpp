#include "textureArray.hpp"

#include <memory>

#include <GL/glew.h>
#include <GL/gl.h>

#include "../utils/utils.hpp"

TextureArray::TextureArray(const std::shared_ptr<const GLuint> &id, const int count, const int width, const int height, const int nChannels) :
    id(id), count(count), width(width), height(height), nChannels(nChannels)
{

}

TextureArray::~TextureArray() {

}

TextureArray * TextureArray::createTexture(const int count, const int width, const int height, const int nChannels) {
    if(count < 1) {
        return nullptr;
    }

    if(width < 1) {
        return nullptr;
    }

    if(height < 1) {
        return nullptr;
    }

    if(nChannels < 1) {
        return nullptr;
    }

    GLuint *id = new GLuint;
    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, *id);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,
                 width, height, count, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Enable comparison mode for shadow sampling (if using sampler2DShadow)
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    std::shared_ptr<GLuint> ID(id, deleteTexture);

    return new TextureArray(ID, count, width, height, nChannels);
}

void TextureArray::bind(const GLuint loc, const int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, *id);
    glUniform1i(loc, slot);
}

void TextureArray::bindToFrameBuffer(const int layer) const {
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *id, 0, layer);
}

void TextureArray::unbindFromFrameBuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}