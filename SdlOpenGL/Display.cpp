#include "Display.hpp"

// constructor
Display::Display(const char *winTitle, const int w, const int h)
{
	// initialize the SDL subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "Display::Display: Failed to init SDL!\n";
		return;
	}

	// create the app window
	win = SDL_CreateWindow(
		winTitle,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		w, h,
		SDL_WINDOW_OPENGL
	);

	if (!win) {
		std::cerr << "Display::Display: Failed to create SDL window!\n";
		return;
	}

	// set opengl properties
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE
	);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// create an opengl context
	gc = SDL_GL_CreateContext(win);

}

// deconstructor
Display::~Display(void)
{
	// cleanup
	SDL_GL_DeleteContext(gc);
	if (win) SDL_DestroyWindow(win);
	SDL_Quit();
}

// swaps buffers
void Display::update(void)
{
	SDL_GL_SwapWindow(win);
}