#ifndef VERTEX_BUFFER_H_INCLUDED
#define VERTEX_BUFFER_H_INCLUDED

#include <cstdint>
#include <GL/glew.h>

struct VertexColored
{
    float x,y,z;
    float nx,ny,nz;
    float r,g,b;
};

struct VertexTextured
{
    float x,y,z;
    float nx,ny,nz;
    float u,v;
};

// for ImGUI,
// matches struct ImDrawVert
struct VertexTexCol2d
{
    float x,y;
    float u,v;
    uint32_t color;
};

class VertexBuffer
{
    friend class Renderer;

public:

    enum Type
    {
        POS_COLOR, // data is in format of VertexColored struct
        POS_TEXCOORD, // data is in format of VertexTextured struct
        POS_TEX_COLOR_2D, // data is in format of VertexTexCol2d struct
        UNINITIALIZED
    };

    enum Usage
    {
        USAGE_STATIC, // this vertex buffer won't often be updated
        USAGE_DYNAMIC // this vertex buffer will be updated frequently
    };

    /**
     * @brief Create the internal buffer and store the given vertex data
     * 
     * @param type the type of vertex data contained in the buffer
     * @param vertices pointer array of one of the VertexColored/VertexTextured struct data formats
     * @param verticesSize the number of VertexColored/VertexTextured structs in vertices
     * @param indices pointer to indices for the buffer, or null
     * @param indicesSize size of the indices array (0 if none)
     * @param usage how often new data will be given to this vertex buffer to use
     * @return true on success, false on failure
     */
    VertexBuffer(
        Type type,
        void* vertices,
        size_t verticesSize,
        uint32_t* indices,
        size_t indicesSize,
        Usage usage = USAGE_STATIC
    );
    ~VertexBuffer();

    /**
     * @brief store new input data in the vertex buffer
     * 
     * @param vertices the new data to store
     * @param verticesSize the number of VertexColored/VertexTextured structs in vertices.
     *      must be same size as what was given in constructor
     * @return true on success, false on failure
     */
    bool UpdateVertices( void* vertices, size_t verticesSize );

private:
    Type mType;
    
    GLuint mVAO, mVBO, mEBO;
    
    size_t mNumVertices; // number of vertices in the buffer
    size_t mVertexStride; // size of 1 vertex in bytes
    size_t mNumIndices; // number of indices in the buffer
};

#endif

