#include "Renderer.h"

// Constructor
Renderer::Renderer( const char *title, const int width, const int height )
{
    // internal 'window should close' boolean
    mQuit = false;

    // init SDL
    if ( SDL_Init(SDL_INIT_VIDEO < 0) ) {
        std::cerr << "Renderer::Renderer: failed to init sdl: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // create the app window
    mWindow = SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL );
    if ( !mWindow ) {
        std::cerr << "Renderer::Renderer: failed to create window: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // set opengl options
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    // create a render context
    mContext = SDL_GL_CreateContext( mWindow );

    // enable vsync
    SDL_GL_SetSwapInterval( 1 );

    // init glew
    glewInit();

}

Renderer::~Renderer()
{
    //glfwTerminate();
    SDL_GL_DeleteContext( mContext );
    SDL_DestroyWindow( mWindow );
    SDL_Quit();
}

void Renderer::Clear()
{
    glClear( GL_COLOR_BUFFER_BIT );
}

void Renderer::Update()
{
    SDL_Event e;
    while ( SDL_PollEvent(&e) )
    {
        if ( e.type == SDL_QUIT ) mQuit = true;
    }

    SDL_GL_SwapWindow( mWindow );
}

bool Renderer::ShouldClose()
{
    return mQuit;
}

