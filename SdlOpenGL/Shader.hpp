// Header for shader class
#include <GL/glew.h>
#include <GL/GL.h>
#include <iostream>
#include <fstream>
#include <sstream>

#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

class Shader
{
public:
	// Constructor
	Shader(std::string vFile, std::string fFile);

	// Deconstructor
	~Shader(void);

	void use(void);
private:
	GLuint vertexShaderID;
	GLuint fragmentShaderID;

	GLuint progID; // the final shader program id

	// loads a single shader
	GLint loadShader(std::string fname, int shaderType);
};

#endif // SHADER_H_INCLUDED
