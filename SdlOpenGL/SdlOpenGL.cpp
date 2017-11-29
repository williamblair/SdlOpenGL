// SdlOpenGL.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <vector>

#include <GL/glew.h>

#include "Shader.hpp"
#include "Display.hpp"

#include <SDL.h>
#undef main

//SDL_Window    *win = NULL;
//SDL_GLContext  gc;

//bool initSDL(void);
bool initOpenGL(void);

bool mainLoop(Display &d);


int main(int argc, char *argv[])
{
	//if (!initSDL()) return -1;

	Display display("SDL OpenGL", 640, 480);

	if (!initOpenGL()) return -1;

	Shader s("../SdlOpenGL/VertexShader.glsl", "../SdlOpenGL/FragmentShader.glsl");

	mainLoop(display);

	// cleanup
	//SDL_GL_DeleteContext(gc);
	//if (win) SDL_DestroyWindow(win);
	//SDL_Quit();
    return 0;
}

/*
bool initSDL(void)
{
	// initialize the SDL subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "Failed to init SDL!\n";
		return false;
	}

	// create the app window
	win = SDL_CreateWindow(
		"OpenGL SDL",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640, 480,
		SDL_WINDOW_OPENGL
	);

	if (!win) {
		std::cerr << "Failed to create SDL window!\n";
		return false;
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

	return true;
}
*/
bool initOpenGL(void)
{
	// initialize glew
	glewInit();

	// set the background color
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	// clear the context
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// switch double buffers
	//SDL_GL_SwapWindow(win);

	return true;
}
/*
void createVBO(void)
{
	// initialize the object within opengl
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// send it our data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*triData.size(), triData.data(), GL_STATIC_DRAW);
	
	// unbind the data
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void createVAO(void)
{
	// initialize the object within opengl
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// send it our data
	//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*triData.size(), triData.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(GLfloat),
		(GLvoid*)0
	);

	// unbind the data
	glEnableVertexAttribArray(0);
	//glBindVertexArray(0);
}
*/
bool mainLoop(Display &d)
{
	SDL_Event e;

	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT) quit = true;
			else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = true;
			}
		}

		// do opengl stuff
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use our shader!
		//s.use();

		// draw our stuff
		//glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindVertexArray(0);

		// update the screen
		//SDL_GL_SwapWindow(win);
		d.update();

		// delay a bit
		SDL_Delay(1000.0f / 60.0f);
	}

	return true;
}


