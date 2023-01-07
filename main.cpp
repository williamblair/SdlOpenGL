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
#include "AssimpMesh.h"
#include "GameTimer.h"

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
    Renderer& render = *Renderer::GetInstance();
    render.Init( "SDL2 Window", 640, 480 );
    GameTimer gameTimer;
    AssimpMesh asmpMesh( "data/Woman.gltf" );
    Texture asmpTex( "data/Woman.png" );

    asmpMesh.SetTexture( &asmpTex, 0 );
    asmpMesh.SetPosition( glm::vec3(1.0f,-2.0f,-3.0f) );
    asmpMesh.SetScale( glm::vec3(0.005f,0.005f,0.005f) );

    while ( !render.ShouldClose() )
    {
        float dt = gameTimer.Update();
        asmpMesh.Update( dt );

        glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
        render.Clear();
        asmpMesh.Draw();
        render.Update();
    }

    return 0;
}

