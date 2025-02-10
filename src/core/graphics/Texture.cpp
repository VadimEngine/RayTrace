// standard lib
#include <stdexcept>
// third party
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
// program
#include "core/Graphics/Texture.h"

Texture::Texture(const unsigned char* textureData, int width, int height, int channels) {
    mWidth_ = width;
    mHeight_ = height;
    mChannels_ = channels;
    mTextureId_ = genGLTexture(textureData, width, height, channels);
}

Texture::Texture(ImageData& imageData) {
    mWidth_ = imageData.width;
    mHeight_ = imageData.height;
    mChannels_ = imageData.channels;
    mTextureId_ = genGLTexture(imageData.pixels, imageData.width, imageData.height, imageData.channels);
}

unsigned int Texture::getId() const {
    return mTextureId_;
}

unsigned int Texture::genGLTexture(const unsigned char* textureData, int width, int height, int channels) {
    if (textureData == nullptr) {
        throw 
        std::runtime_error("Texture build failed");
    }

    unsigned int textureId;

    GLenum format;
    if (channels == 1) {
        format = GL_RED;
    } else if (channels == 3) {
        format = GL_RGB;
    } else if (channels == 4) {
        format = GL_RGBA;
    }
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureId;
}