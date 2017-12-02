//#include "glad/glad.h"
#include <GL/glew.h>

#include <vector>
#include <iostream>

#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

class Object
{
public:

    // constructor/deconstructor
    Object(std::vector<GLfloat> &vertices);
    ~Object(void);

    // draw the vertices
    void draw(void);

private:
    std::vector<GLfloat> vertices; 

    GLuint VAO, VBO;
};

#endif

