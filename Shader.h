#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:

    // Constructor/Deconstructor
    Shader( const std::string vertexFile, const std::string shaderFile );
    ~Shader();

    // tell opengl to use our shader
    void Use();

    // Set shader uniforms; returns true on success, false on failure
    bool SetFloat( const std::string& name, const float val );
    bool SetVec3( const std::string& name, const glm::vec3& val );
    bool SetMat4( const std::string& name, const glm::mat4& val );

    // return the program ID
    GLuint GetProgID();

private:

    std::string getShaderStr( const std::string filename );

    GLuint mProgID;

};

#endif

