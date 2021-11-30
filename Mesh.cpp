#include "Mesh.h"
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define FLOATS_PER_VERTEX 8

// constructor
Mesh::Mesh( std::vector<GLfloat>& vertices, GLuint shaderID)
{
    // copy in the data
    this->mVertices = vertices;

    // set the associated shader id
    this->mShaderID = shaderID;
    
    mTransformLoc = glGetUniformLocation( this->mShaderID, "transform" );

    // default the transformation to identity
    mTransform = glm::mat4(1.0f);

    // generate the array and buffer objects
    glGenVertexArrays( 1, &mVAO );
    glGenBuffers( 1, &mVBO );

    glBindVertexArray( mVAO );
    
        glBindBuffer( GL_ARRAY_BUFFER, mVBO );
        glBufferData( GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW );

        // Vertex Attribute
        // ---------------------------------------------------------------------------
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX*sizeof(GLfloat), (void*)0 );
        glEnableVertexAttribArray( 0 ); // location=0
        
        // Color Attribute
        // ---------------------------------------------------------------------------
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)) );
        glEnableVertexAttribArray( 1 ); // location=1

        // Texture Attribute
        // ---------------------------------------------------------------------------
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
        glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)) );
        glEnableVertexAttribArray( 2 ); // location=1

        glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindVertexArray( 0 );
}

Mesh::~Mesh(void)
{
    glDeleteVertexArrays( 1, &mVAO );
    glDeleteBuffers( 1, &mVBO );
}

void Mesh::Draw(void)
{
    //assert( mVertices.size() % FLOATS_PER_VERTEX == 0 );

    // set the transformation matrix value
    glUniformMatrix4fv( mTransformLoc, 1, GL_FALSE, glm::value_ptr(mTransform) );

    glBindTexture( GL_TEXTURE_2D, mTextureID );
    glBindVertexArray( mVAO );
    glDrawArrays( GL_TRIANGLES, 0, mVertices.size() / FLOATS_PER_VERTEX );
}

bool Mesh::LoadTexture( std::string fname )
{
    // load the data
    int width, height, nChannels;
    unsigned char* data = stbi_load( fname.c_str(), &width, &height, &nChannels, 0 );

    if ( !data ) {
        std::cerr << "Mesh::loadTexture: Failed to load image: " << fname << std::endl;
        return false;
    }

    // Generate the texture memory
    //GLuint textureID;
    glGenTextures( 1, &mTextureID );
    glBindTexture( GL_TEXTURE_2D, mTextureID );

    // set scaling options
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // send the data to opengl
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
    glGenerateMipmap( GL_TEXTURE_2D );

    stbi_image_free( data );
    
    return true;
}

bool Mesh::Rotate( GLfloat angle, glm::vec3 axis )
{    
    // set the new transformation matrix and set its value in the shader
    mTransform = glm::rotate( angle, axis );
    unsigned int mTransformLoc = glGetUniformLocation( mShaderID, "transform" );
    glUniformMatrix4fv( mTransformLoc, 1, GL_FALSE, glm::value_ptr(mTransform) );

    return true;
}

