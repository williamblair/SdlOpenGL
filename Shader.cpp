#include "Shader.h"

// Constructor
Shader::Shader( const std::string vertexFile, const std::string shaderFile )
{
    // get the string data of both files
    std::string vFileStr = getShaderStr( vertexFile );
    std::string fFileStr = getShaderStr( shaderFile );

    // initialze each shader
    GLuint vertexShaderID   = glCreateShader( GL_VERTEX_SHADER );
    GLuint fragmentShaderID = glCreateShader( GL_FRAGMENT_SHADER );

    const char* shaderSrc = vFileStr.c_str();

    // send and compile the vertex shader
    glShaderSource( vertexShaderID, 1, &shaderSrc, NULL);
    glCompileShader( vertexShaderID );

    // check for success
    int success;
    char infoLog[512];
    glGetShaderiv( vertexShaderID, GL_COMPILE_STATUS, &success );
    if ( !success ) {
        glGetShaderInfoLog( vertexShaderID, 512, NULL, infoLog );
        std::cerr << "Shader::Shader: Vertex Shader Compilation failed: "
            << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }

    shaderSrc = fFileStr.c_str();
    
    // send and compile the fragment shader
    glShaderSource( fragmentShaderID, 1, &shaderSrc, NULL );
    glCompileShader( fragmentShaderID );

    // check for success
    glGetShaderiv( fragmentShaderID, GL_COMPILE_STATUS, &success );
    if ( !success ) {
        glGetShaderInfoLog( fragmentShaderID, 512, NULL, infoLog );
        std::cerr << "Shader::Shader: Fragment Shader Compilation failed: "
            << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }

    // link the shaders to the main program
    mProgID = glCreateProgram();
    glAttachShader( mProgID, vertexShaderID );
    glAttachShader( mProgID, fragmentShaderID );
    glLinkProgram( mProgID);

    // check for success
    glGetShaderiv( mProgID, GL_LINK_STATUS, &success );
    if ( !success ) {
        glGetShaderInfoLog( mProgID, 512, NULL, infoLog );
        std::cerr << "Shader::Shader: Program Linkage Failed: "
            << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }

    // delete our component shaders now
    glDeleteShader( vertexShaderID );
    glDeleteShader( fragmentShaderID );
    
}

void Shader::Use(void)
{
    glUseProgram( mProgID );
}

// deconstructor
Shader::~Shader(void)
{}

std::string Shader::getShaderStr( const std::string filename )
{
    std::ifstream f( filename, std::ios::in );
    if ( !f.is_open() ) {
        std::cerr << "Shader::getShaderStr: failed to open " << filename << std::endl;
        return "";
    }

    std::string str((std::istreambuf_iterator<char>(f)),
                 std::istreambuf_iterator<char>());

    return str;
}

GLuint Shader::GetProgID()
{
    return mProgID;
}

