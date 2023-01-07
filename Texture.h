#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <string>

#include <GL/glew.h>

class Texture
{
    friend class Renderer;

public:

    Texture( const std::string& fileName);
    ~Texture();
    
    GLuint GetID(void) const { return mTextureID; }
    int GetWidth(void) const { return mWidth; }
    int GetHeight(void) const { return mHeight; }
    int GetNumChannels(void) const { return mNumChannels; }

private:
    GLuint mTextureID;
    int mWidth, mHeight;
    int mNumChannels;
};

#endif

