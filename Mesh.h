#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

class Mesh
{
public:

    // constructor/deconstructor
    Mesh( std::vector<GLfloat>& vertices, GLuint shaderID );
    ~Mesh();

    // draw the vertices
    void Draw();

    // load the given texture and store it
    bool LoadTexture( std::string fname );

    // angle is expected in radians (use glm::radians)
    bool Rotate( GLfloat angle, glm::vec3 axis );

private:
    std::vector<GLfloat> mVertices; 

    GLuint mTextureID;

    GLuint mVAO, mVBO;

    glm::mat4 mTransform; // how to rotate/move the object

    GLuint mShaderID; // the shader id associated with this object

    GLuint mTransformLoc; // uniform location to set the transformation matrix
};

#endif

