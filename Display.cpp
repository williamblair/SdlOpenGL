#include "Display.hpp"

// callbacks
//void framebuffer_size_callback(GLFWwindow *window, int width, int height)
//{
//    glViewport(0,0, width, height);
//}

// Constructor
Display::Display(const char *title, const int width, const int height)
{
    // init GLFW
/*    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create the window
    window = NULL;
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!window) {
        std::cerr << "Display::Display: failed to create GLFW window!\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // use the opengl context on the window
    glfwMakeContextCurrent(window);

    // set the framebuffer size callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // use glad to load all opengl function pointers
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Display::Display: failed to intialize GLAD\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
*/
    quit = false;

    // init SDL
    if(SDL_Init(SDL_INIT_VIDEO < 0)) {
        std::cerr << "Display::Display: failed to init sdl: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // create the app window
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    if(!window) {
        std::cerr << "Display::Display: failed to create window: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // set opengl options
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // create a render context
    gc = SDL_GL_CreateContext(window);

    // enable vsync
    SDL_GL_SetSwapInterval(1);

    // init glew
    glewInit();

}

Display::~Display(void)
{
    //glfwTerminate();
    SDL_GL_DeleteContext(gc);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::clear(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Display::update(void)
{
    //glfwSwapBuffers(window);
    //glfwPollEvents();

    //if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    //    glfwSetWindowShouldClose(window, true);

    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        if(e.type == SDL_QUIT) quit = true;
    }

    SDL_GL_SwapWindow(window);
}

bool Display::shouldClose(void)
{
    //return glfwWindowShouldClose(window);
    return quit;
}

