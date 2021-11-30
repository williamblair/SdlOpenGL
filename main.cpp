#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Shader.hpp"
#include "Mesh.h"


int main()
{

    Renderer render( "SDL2 Window", 640, 480 );


    Shader shader( "VertexShader.glsl", "FragmentShader.glsl" );

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<GLfloat> vertices = {
        // vertices            // color             // texture coords
        -0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // bottom left
         0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // bottom right
         0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,     0.5f, 1.0f  // top
    };

    Mesh mesh( vertices, shader.getProgID() );
    mesh.LoadTexture( "wall.jpg" );

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    mesh.Rotate( glm::radians(45.0f), glm::vec3(0.0f,0.0f,1.0f) );

    // render loop
    // -----------
    while ( !render.ShouldClose() )
    {
        glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
        render.Clear();

        // draw our first triangle
        shader.use();
        mesh.Draw();
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        render.Update();
    }

    return 0;
}

