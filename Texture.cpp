#include <cstdlib>
#include <iostream>

#include "stb_image.h"
#include "Texture.h"

Texture::Texture( const std::string& fileName )
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load( fileName.c_str(), &mWidth, &mHeight, &mNumChannels, 0 );
    if ( !data ) {
        std::cerr << "Texture::Texture failed to load " << fileName << std::endl;
        mWidth = 0;
        mHeight = 0;
        exit( EXIT_FAILURE );
    }

    // generate texture memory
    glGenTextures( 1, &mTextureID );
    glBindTexture( GL_TEXTURE_2D, mTextureID );

    // set scaling options
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // send the data to OpenGL
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        mNumChannels == 3 ? GL_RGB : GL_RGBA,
        mWidth, mHeight,
        0,
        mNumChannels == 3 ? GL_RGB : GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );
    glGenerateMipmap( GL_TEXTURE_2D );

    stbi_image_free( data );
}

Texture::~Texture()
{
    if ( mWidth != 0 || mHeight != 0 ) {
        glDeleteTextures( 1, &mTextureID );
    }
}

