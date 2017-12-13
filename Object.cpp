#include "Object.hpp"

// constructor
Object::Object(std::vector<GLfloat> &vertices)
{
    // copy in the data
    this->vertices = vertices;

    // generate the array and buffer objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        // Vertex Attribute
        // ---------------------------------------------------------------------------
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0); // location=0
        
        // Color Attribute
        // ---------------------------------------------------------------------------
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glEnableVertexAttribArray(1); // location=1

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

Object::~Object(void)
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Object::draw(void)
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
}


