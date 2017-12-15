//#include "glad/glad.h"
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

class Object
{
public:

    // constructor/deconstructor
    Object(std::vector<GLfloat> &vertices, GLuint shaderID);
    ~Object(void);

    // draw the vertices
    void draw(void);

    // load the given texture and store it
    bool loadTexture(std::string fname);

    // angle is expected in radians (use glm::radians)
    bool rotate(GLfloat angle, glm::vec3 axis);

private:
    std::vector<GLfloat> vertices; 

    GLuint textureID;

    GLuint VAO, VBO;

    glm::mat4 transform; // how to rotate/move the object

    GLuint shaderID; // the shader id associated with this object

    GLuint transformLoc; // uniform location to set the transformation matrix
};

#endif

