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

int main()
{
    Renderer& render = *Renderer::GetInstance();
    render.Init( "SDL2 Window", 640, 480 );
    GameTimer gameTimer;
    AssimpMesh asmpMesh( "data/Woman.gltf" );
    Texture asmpTex( "data/Woman.png" );

    asmpMesh.SetTexture( &asmpTex, 0 );
    asmpMesh.SetPosition( glm::vec3(0.0f,-1.0f,-3.0f) );
    asmpMesh.SetScale( glm::vec3(0.005f,0.005f,0.005f) );

    Renderer::DirectionalLight dirLight = {
        glm::vec3(0.0f,-1.0f,-0.25f),
        glm::vec3(1.0f,1.0f,1.0f)
    };
    render.SetDirLight( dirLight, 0 );

    float modelRot = 0.0f;
    while ( !render.ShouldClose() )
    {
        float dt = gameTimer.Update();
        asmpMesh.Update( dt );
        modelRot += dt * 90.0f;
        asmpMesh.SetRotation(
            glm::quat( glm::vec3(0.0f,glm::radians(modelRot),0.0f) )
        );

        glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
        render.Clear();
        asmpMesh.Draw();
        render.Update();
    }

    return 0;
}

