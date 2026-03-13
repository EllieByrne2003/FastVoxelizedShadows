#include "texture.hpp"

#include <memory>
#include <mutex>
#include <map>

#include <GL/glew.h>
#include <GL/gl.h>

#include "../utils/utils.hpp"

#define TEXTURE_DIR std::string("res/textures")

Texture::Texture(const std::shared_ptr<const GLuint> &id, const int width, const int height, const int nChannels) :
    id(id), width(width), height(height), nChannels(nChannels)
{

}

Texture::~Texture() {

}

Texture * Texture::readTexture(const std::string &filename) {
    static std::mutex textureMutex;
    static std::map<std::string, Texture> textureCache;

    std::unique_lock lock(textureMutex);

    if(textureCache.find(filename) != textureCache.end()) {
        return new Texture(textureCache.at(filename));
    } else {
        const std::string filepath = TEXTURE_DIR + "/" + filename;

        GLuint *id = new GLuint;
        int width, height, nChannels;
        *id = ::createTexture(filepath, width, height, nChannels);

        if(*id == 0) {
            delete id;

            return nullptr;
        }

        std::shared_ptr<GLuint> ID(id, deleteTexture);

        textureCache.emplace(filename, Texture(ID, width, height, nChannels));
        return new Texture(textureCache.at(filename));
    }
}


Texture * Texture::createTexture(const int width, const int height, const int nChannels) {
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
    glBindTexture(GL_TEXTURE_2D, *id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Enable comparison mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    // Set the comparison function (e.g., GL_LEQUAL means "pass if ref <= stored depth")
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    // Texture filtering (for PCF you may want linear filtering)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


    
    // Border color used when sampling outside the texture (for directional lights)
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    std::shared_ptr<GLuint> ID(id, deleteTexture);

    return new Texture(ID, width, height, nChannels);
}

void Texture::bind(const GLuint loc, const int slot) const {
    // TODO bounds check, slot must be > 0 and < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, *id);
    glUniform1i(loc, slot);
}

void Texture::bindToFrameBuffer() const {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *id, 0);
}

void Texture::unbindFromFrameBuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
