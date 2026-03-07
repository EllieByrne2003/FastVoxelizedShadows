#include "texture.hpp"

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
        *id = createTexture(filepath, width, height, nChannels);

        if(*id == 0) {
            delete id;

            return nullptr;
        }

        std::shared_ptr<GLuint> ID(id, deleteTexture);

        textureCache.emplace(filename, Texture(ID, width, height, nChannels));
        return new Texture(textureCache.at(filename));
    }
}

void Texture::bind(const int slot) const {
    // TODO bounds check, slot must be > 0 and < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, *id);
}