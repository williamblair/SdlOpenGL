#include <iostream>
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(
    Type type,
    void* vertices,
    size_t verticesSize,
    uint32_t* indices,
    size_t indicesSize,
    Usage usage ) :
        mType( UNINITIALIZED ),
        mVAO( 0 ),
        mVBO( 0 ),
        mEBO( 0 ),
        mNumVertices( 0 ),
        mVertexStride( 0 ),
        mNumIndices( 0 )
{
    int attribLocation; // aPos, where we set location = 0
    int dataType;
    int shouldNormalize;
    int floatsPerVertex;
    void* beginOffset;

    switch ( type )
    {
    case POS_COLOR: mVertexStride = sizeof( VertexColored ); break;
    case POS_TEXCOORD: mVertexStride = sizeof( VertexTextured ); break;
    case POS_TEX_COLOR_2D: mVertexStride = sizeof( VertexTexCol2d ); break;
    default:
        std::cerr << "Unhandled vertex buffer type: " << (int)type << std::endl;
        exit( EXIT_FAILURE );
        break;
    }
    
    glGenVertexArrays( 1, &mVAO );
    glGenBuffers( 1, &mVBO );
    if ( indices != nullptr && indicesSize > 0 )
    {
        glGenBuffers( 1, &mEBO );
    }
    glBindVertexArray( mVAO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );
    glBufferData(
        GL_ARRAY_BUFFER,
        verticesSize * mVertexStride,
        vertices,
        (usage == USAGE_STATIC) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW
    );
    if ( indices != nullptr && indicesSize > 0 )
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mEBO );
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indicesSize * sizeof( uint32_t ),
            indices,
            GL_STATIC_DRAW
        );
    }
    
    mType = type;
    mNumVertices = verticesSize;
    mNumIndices = indicesSize;
    switch (type)
    {
    case POS_COLOR:
        // position
        attribLocation = 0; // aPos, where we set location = 0
        dataType = GL_FLOAT;
        shouldNormalize = GL_FALSE;
        floatsPerVertex = 3; // xyz
        beginOffset = (void*)0;
        glVertexAttribPointer(
            attribLocation,
            floatsPerVertex,
            dataType,
            shouldNormalize,
            mVertexStride,
            beginOffset
        );
        glEnableVertexAttribArray( attribLocation );
        // normal 
        attribLocation = 1; // aNormal, where we set location = 1
        dataType = GL_FLOAT;
        shouldNormalize = GL_FALSE;
        floatsPerVertex = 3; // nx,ny,nz
        beginOffset = (void*)( 3 * sizeof( float )); // skip xyz
        glVertexAttribPointer(
            attribLocation,
            floatsPerVertex,
            dataType,
            shouldNormalize,
            mVertexStride,
            beginOffset
        );
        glEnableVertexAttribArray( attribLocation );
        
        // color
        attribLocation = 2; // aColor, where we set location = 2
        dataType = GL_FLOAT;
        shouldNormalize = GL_FALSE;
        floatsPerVertex = 3; // rgb
        beginOffset = (void*)( 6 * sizeof( float )); // skip xyz, nxnynz
        glVertexAttribPointer(
            attribLocation,
            floatsPerVertex,
            dataType,
            shouldNormalize,
            mVertexStride,
            beginOffset
        );
        glEnableVertexAttribArray( attribLocation );
        break;
    case POS_TEXCOORD:
        // position
        attribLocation = 0; // aPos, where we set location = 0
        dataType = GL_FLOAT;
        shouldNormalize = GL_FALSE;
        floatsPerVertex = 3; // xyz
        beginOffset = (void*)0;
        glVertexAttribPointer(
            attribLocation,
            floatsPerVertex,
            dataType,
            shouldNormalize,
            mVertexStride,
            beginOffset
        );
        glEnableVertexAttribArray( attribLocation );
        // normal 
        attribLocation = 1; // aNormal, where we set location = 1
        dataType = GL_FLOAT;
        shouldNormalize = GL_FALSE;
        floatsPerVertex = 3; // nx,ny,nz
        beginOffset = (void*)( 3 * sizeof( float )); // skip xyz
        glVertexAttribPointer(
            attribLocation,
            floatsPerVertex,
            dataType,
            shouldNormalize,
            mVertexStride,
            beginOffset
        );
        glEnableVertexAttribArray( attribLocation );
        
        // tex coord
        attribLocation = 2; // aTexCoord, where we set location = 2
        dataType = GL_FLOAT;
        shouldNormalize = GL_FALSE;
        floatsPerVertex = 2; // uv
        beginOffset = (void*)( 6 * sizeof( float )); // skip xyz, nxnynz
        glVertexAttribPointer(
            attribLocation,
            floatsPerVertex,
            dataType,
            shouldNormalize,
            mVertexStride,
            beginOffset
        );
        glEnableVertexAttribArray( attribLocation );
        break;
    case POS_TEX_COLOR_2D:
        // position
        attribLocation = 0; // aPos, where we set location = 0
        dataType = GL_FLOAT;
        shouldNormalize = GL_FALSE;
        floatsPerVertex = 2; // xy
        beginOffset = (void*)0;
        glVertexAttribPointer(
            attribLocation,
            floatsPerVertex,
            dataType,
            shouldNormalize,
            mVertexStride,
            beginOffset
        );
        glEnableVertexAttribArray( attribLocation );
        // tex coord
        attribLocation = 1; // aTexCoord, where we set location = 1
        dataType = GL_FLOAT;
        shouldNormalize = GL_FALSE;
        floatsPerVertex = 2; // uv
        beginOffset = (void*)( 2 * sizeof( float )); // skip xy
        glVertexAttribPointer(
            attribLocation,
            floatsPerVertex,
            dataType,
            shouldNormalize,
            mVertexStride,
            beginOffset
        );
        glEnableVertexAttribArray( attribLocation );
        // color
        attribLocation = 2; // aColor, where we set location = 2
        dataType = GL_UNSIGNED_BYTE; // 4 bytes per uint32_t
        shouldNormalize = GL_TRUE; // auto convert from unsigned byte to float
        floatsPerVertex = 4; // color
        beginOffset = (void*)( 4 * sizeof( float )); // skip xy,uv
        glVertexAttribPointer(
            attribLocation,
            floatsPerVertex,
            dataType,
            shouldNormalize,
            mVertexStride,
            beginOffset
        );
        glEnableVertexAttribArray( attribLocation );
        break;
    default:
        std::cerr << "Unhandled VertexBuffer type: " << (int)type << std::endl;
        exit( EXIT_FAILURE );
        break;
    }
    
    mNumVertices = verticesSize;
    
    // unbind the current buffers
    // ORDER MATTERS - the VAO must be unbinded FIRST!
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    if ( indices != nullptr && indicesSize > 0 )
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }
}

VertexBuffer::~VertexBuffer()
{
    if ( mNumVertices != 0 ) {
        glDeleteBuffers( 1, &mVBO );
    }
    if ( mNumIndices != 0 ) {
        glDeleteBuffers( 1, &mEBO );
    }
    if ( mVAO != 0 ) {
        glDeleteVertexArrays( 1, &mVAO );
    }
}

bool VertexBuffer::UpdateVertices( void* vertices, size_t verticesSize )
{
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        verticesSize * mVertexStride,
        vertices
    );
    return true;
}

