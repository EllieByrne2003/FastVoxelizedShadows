#include "utils.hpp"

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../../lib/stb_image.h"


GLuint createTexture(const std::string &path, int &width, int &height, int &nChannels) {
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nChannels, 0);
    if(!data) {
        std::cerr << "Failed to load: " << path << std::endl;
        return 0;
    }

    GLuint textureID; // TODO delete when error occurs
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    if(nChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    } else if(nChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    } else if(nChannels == 1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    } else {
        stbi_image_free(data);
        std::cerr << path << " has an unsupported number of channels." << std::endl;
        return 0;
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

GLuint createCubeMap(const std::vector<std::string> &paths) {
    if(paths.size() != 6) {
        return 0; // TODO error message
    }

    GLuint cubeMapID; // TODO delete properly
    glGenTextures(1, &cubeMapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);

    for(int i = 0; i < paths.size(); i++) {
	    int width, height, nChannels;

	    unsigned char *data = stbi_load(paths[i].c_str(), &width, &height, &nChannels, 0);
	    if(!data)
	    {
		    std::cerr << "Failed to load: " << paths[i] << std::endl;
		    return 0;
	    }

        if(nChannels == 4) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        } else if(nChannels == 3) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        } else {
            stbi_image_free(data);
            std::cerr << paths[i] << " has an unsupported number of channels." << std::endl;
            return 0;
        }
    }   

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return cubeMapID;
}