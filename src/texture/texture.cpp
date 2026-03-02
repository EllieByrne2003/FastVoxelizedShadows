#include "texture.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

Texture::Texture(const std::shared_ptr<const GLuint> &id, const int width, const int height, const int nChannels) :
    id(id), width(width), height(height), nChannels(nChannels)
{
    
}

Texture::~Texture() {

}

void Texture::bind(const int slot) const {
    // TODO bounds check, slot must be > 0 and < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, *id);
}