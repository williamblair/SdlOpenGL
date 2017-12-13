#include "Object.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0); // location=0
        
        // Color Attribute
        // ---------------------------------------------------------------------------
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glEnableVertexAttribArray(1); // location=1

        // Texture Attribute
        // ---------------------------------------------------------------------------
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
        glEnableVertexAttribArray(2); // location=1

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
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
}

bool Object::loadTexture(std::string fname)
{
    // load the data
    int width, height, nChannels;
    unsigned char *data = stbi_load(fname.c_str(), &width, &height, &nChannels, 0);

    if(!data) {
        std::cerr << "Object::loadTexture: Failed to load image: " << fname << std::endl;
        return false;
    }

    // Generate the texture memory
    //GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // set scaling options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // send the data to opengl
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    
    return true;
}


