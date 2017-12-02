/* Display Class */

#include <iostream>
#include <GL/glew.h>

//#include "glad/glad.h"
//#include <GLFW/glfw3.h>
#include <SDL2/SDL.h>

#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

// callback funcs
//void framebuffer_size_callback(GLFWwindow *window, int width, int height);

class Display
{
public:

    // Constructor / Deconstructor
    Display(const char *title, const int width, const int height);
    ~Display(void);

    // clear the screen
    void clear(void);

    // test if the window should close
    bool shouldClose(void);

    // Poll events and
    // update the screen
    void update(void);


private:

    //GLFWwindow *window;
    SDL_Window *window;
    SDL_GLContext gc;
    bool quit;
    
};

#endif

