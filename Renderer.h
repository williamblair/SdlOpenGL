#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <iostream>
#include <GL/glew.h>

#include <SDL2/SDL.h>

/* Renderer Class */
class Renderer
{
public:

    // Constructor / Deconstructor
    Renderer( const char* title, const int width, const int height );
    ~Renderer();

    // clear the screen
    void Clear();

    // test if the window should close
    bool ShouldClose();

    // Poll events and
    // update the screen
    void Update();


private:

    SDL_Window* mWindow;
    SDL_GLContext mContext;
    bool mQuit;
    
};

#endif

