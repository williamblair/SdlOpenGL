#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
//#include "glad/glad.h"

class Shader
{
public:

    // Constructor/Deconstructor
    Shader(const std::string vertexFile, const std::string shaderFile);
    ~Shader(void);

    // tell opengl to use our shader
    void use(void);

private:

    std::string getShaderStr(const std::string filename);

    GLuint progID;

};

#endif

