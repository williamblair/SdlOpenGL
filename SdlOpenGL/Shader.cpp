// implementation file for shader class
#include "Shader.hpp"

GLint Shader::loadShader(std::string fname, int shaderType)
{
	GLint sid = 0;

	std::ifstream f(fname);
	if (!f.is_open()) {
		std::cerr << "Shader::loadShader: failed to open"
			<< fname << std::endl;
		return 0;
	}

	std::string fdata; // holds the entire file contents as a string
	std::string line; // the current line in the file
	while (!f.eof())
	{
		std::getline(f, line);
		fdata += line + "\n";
	}
	std::cout << fdata << std::endl;
	f.close();

	// compile the shader
	sid = glCreateShader(shaderType);
	const char *strPtr = fdata.c_str();
	glShaderSource(sid, 1, &strPtr, NULL);
	glCompileShader(sid);

	// check for compilation errors
	GLint result = GL_FALSE;
	char errorMessage[500];
	glGetShaderiv(sid, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(sid, 500, NULL, errorMessage);
		std::cerr << "Shader::loadShader: Failed compiling " << fname
			<< ": " << errorMessage << "\n";
	}

	return sid;
}

// Constructor
Shader::Shader(std::string vFile, std::string fFile)
{
	GLint vsid = loadShader(vFile, GL_VERTEX_SHADER);
	GLint fsid = loadShader(fFile, GL_FRAGMENT_SHADER);

	// link the vertex and fragment shaders to our program
	progID = glCreateProgram();
	glAttachShader(progID, vsid);
	glAttachShader(progID, fsid);

	// register the shader with opengl
	glLinkProgram(progID);

	// check for compilation errors
	GLint result = GL_FALSE;
	char errorMessage[2048];
	glGetProgramiv(progID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(progID, 2048, NULL, errorMessage);
		std::cerr << "Shader::loadShader: Failed Linking Shader Program: " 
			<< errorMessage << "\n";
	}

	// delete our temp vertex and fragment shaders
	glDeleteShader(vsid);
	glDeleteShader(fsid);
}

// Decontsructor
Shader::~Shader(void)
{
	glUseProgram(0);
	glDeleteProgram(progID);
}

void Shader::use(void)
{
	glUseProgram(progID);
}
