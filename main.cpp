#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"
#include "cgltf.h"

#ifdef WIN32
#undef main
#endif

class GLTFMesh
{
public:
    GLTFMesh() :
        mData( nullptr )
    {
    }

    ~GLTFMesh()
    {
        if ( mData ) {
            cgltf_free( mData );
            mData = nullptr;
        }
    }

    bool Load(const char* filename)
    {
        cgltf_options options;
        memset(&options, 0, sizeof(options));

        cgltf_result result = cgltf_parse_file(&options, filename, &mData);
        if (result != cgltf_result_success) {
            printf("Failed to load gltf %s\n", filename);
            return false;
        }

        result = cgltf_load_buffers(&options, mData, filename);
        if (result != cgltf_result_success) {
            cgltf_free(mData);
            printf("Failed to load buffers for gltf %s\n", filename);
            return false;
        }

        result = cgltf_validate(mData);
        if (result != cgltf_result_success) {
            cgltf_free(mData);
            printf("Failed to validate file %s\n", filename);
            return false;
        }

        return true;
    }

private:
    cgltf_data* mData;
};

int main()
{

    Renderer render( "SDL2 Window", 640, 480 );


    Shader shader( "shaders/VertexShader.glsl", "shaders/FragmentShader.glsl" );

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<GLfloat> vertices = {
        // vertices            // color             // texture coords
        -0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // bottom left
         0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // bottom right
         0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,     0.5f, 1.0f  // top
    };

    Mesh mesh( vertices, shader.GetProgID() );
    mesh.LoadTexture( "data/wall.jpg" );

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    mesh.Rotate( glm::radians(45.0f), glm::vec3(0.0f,0.0f,1.0f) );

    GLTFMesh gltf;
    if ( !gltf.Load("data/Woman.gltf") ) {
        return 1;
    }

    // render loop
    // -----------
    while ( !render.ShouldClose() )
    {
        glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
        render.Clear();

        // draw our first triangle
        shader.Use();
        mesh.Draw();
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        render.Update();
    }

    return 0;
}

