#pragma once
#include "Core/Utils.h"

class Texture {
public:
    Texture(const unsigned char* textureData, int width, int height, int channels);

    Texture(ImageData& imageData);

    unsigned int getId() const;

private:

    static unsigned int genGLTexture(const unsigned char* textureData, int width, int height, int channels);

    /** GL Texture Id*/
    unsigned int mTextureId_;
    /** Width in pixels*/
    int mWidth_;
    /** Height in pixels*/
    int mHeight_;
    /** Channels per pixel */
    int mChannels_;
};