//#include "glad/glad.h"
//#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Display.hpp"
#include "Shader.hpp"
#include "Object.hpp"


int main()
{

    Display display("GLFW Window", 640, 480);


    Shader shader("VertexShader.glsl", "FragmentShader.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    /*std::vector<GLfloat> vertices = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    };*/
    std::vector<GLfloat> vertices = {
        // vertices            // color             // texture coords
        -0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // bottom left
         0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // bottom right
         0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,     0.5f, 1.0f  // top
    };

    Object object(vertices, shader.getProgID());
    object.loadTexture("wall.jpg");

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    object.rotate(glm::radians(45.0f), glm::vec3(0.0f,0.0f,1.0f));

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

