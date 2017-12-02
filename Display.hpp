/* Display Class */

#include <iostream>
#include <GL/glew.h>

#include <SDL2/SDL.h>

#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED


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

    SDL_Window *window;
    SDL_GLContext gc;
    bool quit;
    
};

#endif

