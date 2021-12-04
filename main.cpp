#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"
#include "GLTFMesh.h"

#ifdef WIN32
#undef main
#endif

glm::mat4 perspMat = glm::perspective(
    45.0f * 3.1415926535f / 180.0f, // FOV
    640.0f / 480.0f, // aspect
    0.1f, // near
    1000.0f // far
);

static bool testMatConstr()
{
    float quatX = 0.259f;
    float quatY = 0.0f;
    float quatZ = 0.0f;
    float quatW = 0.966f;
    glm::vec3 translation( 10.0f, 20.0f, 30.0f );
    glm::quat rotation( quatW, quatX, quatY, quatZ );
    glm::vec3 scale( 2.0f, 1.0f, 0.5f );

    glm::mat4 transformMat = 
        glm::translate( glm::mat4(1.0f), translation ) * 
        glm::mat4_cast( rotation ) *
        glm::scale( glm::mat4(1.0f), scale );

    GLfloat* matPtr = glm::value_ptr( transformMat );
    printf( "Test Result Matrix:\n"
            "%f, %f, %f, %f\n"
            "%f, %f, %f, %f\n"
            "%f, %f, %f, %f\n"
            "%f, %f, %f, %f\n",
        matPtr[0],
        matPtr[1],
        matPtr[2],
        matPtr[3],
        matPtr[4],
        matPtr[5],
        matPtr[6],
        matPtr[7],
        matPtr[8],
        matPtr[9],
        matPtr[10],
        matPtr[11],
        matPtr[12],
        matPtr[13],
        matPtr[14],
        matPtr[15]
    );

    return true;
}

int main()
{

    Renderer render( "SDL2 Window", 640, 480 );


    //Shader shader( "shaders/VertexShader.glsl", "shaders/FragmentShader.glsl" );
    Shader skelTestShader( "shaders/SkelVertShader.glsl", "shaders/SkelFragShader.glsl" );

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
#if 0
    std::vector<GLfloat> vertices = {
        // vertices            // color             // texture coords
        -0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // bottom left
         0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // bottom right
         0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,     0.5f, 1.0f  // top
    };
#endif

    //Mesh mesh( vertices, shader.GetProgID() );
    //mesh.LoadTexture( "data/wall.jpg" );

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //mesh.Rotate( glm::radians(45.0f), glm::vec3(0.0f,0.0f,1.0f) );

    GLTFMesh gltf;
    if ( !gltf.Load(/*"data/Woman.gltf"*/"data/animTri.gltf") ) {
        return 1;
    }
    skelTestShader.Use();
    //gltf.CreateBuffers( skelTestShader.GetProgID() );


    //testMatConstr();

    // render loop
    // -----------
    while ( !render.ShouldClose() )
    {
        glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
        render.Clear();

        // draw our first triangle
        //shader.Use();
        //mesh.Draw();
        //skelTestShader.Use();
        //gltf.Draw( perspMat );
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        render.Update();
    }

    return 0;
}

