#include <iostream>

#include <GL/glew.h>
#include <SDL.h>

#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

class Display
{
public:

	// Constructor
	Display(const char *winTitle, const int w, const int h);

	// Deconstructor
	~Display(void);

	// swaps buffers
	void update(void);

private:
	SDL_Window    *win = NULL;
	SDL_GLContext  gc;
};

#endif // DISPLAY_H_INCLUDED
