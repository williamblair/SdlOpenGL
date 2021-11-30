#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

class Shader
{
public:

    // Constructor/Deconstructor
    Shader( const std::string vertexFile, const std::string shaderFile );
    ~Shader();

    // tell opengl to use our shader
    void Use();

    // return the program ID
    GLuint GetProgID();

private:

    std::string getShaderStr( const std::string filename );

    GLuint mProgID;

};

#endif

