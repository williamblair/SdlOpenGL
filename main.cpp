//#include "glad/glad.h"
//#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "Display.hpp"
#include "Shader.hpp"
#include "Object.hpp"


int main()
{

    Display display("GLFW Window", 640, 480);


    Shader shader("VertexShader.glsl", "FragmentShader.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<GLfloat> vertices = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 

    Object object(vertices);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while(!display.shouldClose())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        display.clear();

        // draw our first triangle
        shader.use();
        object.draw();
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        display.update();
    }

    return 0;
}

