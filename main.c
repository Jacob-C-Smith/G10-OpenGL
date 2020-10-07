#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>

#include <G10/GXEntity.h>

#include <G10/GXMesh.h>
#include <G10/GXOBJ.h>

#include <G10/GXShader.h>

#include <G10/GXTexture.h>
#include <G10/GXBitmap.h>
#include <G10/GXPNG.h>

#include <G10/GXScene.h>
#include <G10/GXCamera.h>

#include <G10/GXilmath.h>

#undef main

int main(int argc, const char* argv[])
{
	// Uninitialized SDL data
	SDL_Window*   window;
	SDL_Event     event;
	SDL_GLContext glContext;
	
	// Uninitialized G10 data
	GXscene_t*    scene;

	// Initialized Data
	u8            running = 1;
	float         d       = 0.f;

	// SDL + GLAD Initialization Junk
	{
		// Initialize SDL
		if (SDL_Init(SDL_INIT_EVERYTHING))
		{
			printf("Failed to initialize SDL\n");
			return -1;
		}
		// Initialize the image loader library
		if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0)
		{
			printf("Failed to initialize SDL Image\n");
			return -1;
		}
		// Create the window
		window = SDL_CreateWindow("G10",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			1280,
			720,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		// Create the OpenGL context
		glContext = SDL_GL_CreateContext(window);
		// Check the window
		if (!window)
		{
			printf("Failed to create a window\n");
			return -1;
		}
		// Check the OpenGL context
		if (!glContext)
		{
			printf("Failed to create an OpenGL Context\n");
			return -1;
		}
		// Check the glad loader
		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{
			printf("Failed to load OpenGL");
			return -1;
		}
	}

	// G10 Initialization Junk
	{
		// Create the scene
		scene = createScene();
		
		// Create the camera
		scene->camera = createCamera((GXvec3_t) { 0.f, 4.f, -1.f }, (GXvec3_t) {0.f,0.f,0.f}, (GXvec3_t) {0.f,1.f,0.f},toRadians(90.f),0.1f,100.f,1280.f/720.f);
		
		// Create the projection matrix
		computeProjectionMatrix(scene->camera);

		// Load and append a test file
		appendEntity(scene, loadEntity("gameassets/cube/.json"));
	}

	// OpenGL Commands
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	GXentity_t* cube = getEntity(scene, "cube");
	cube->transform = createTransform((GXvec3_t){ 0,0,0 }, (GXvec3_t) { 0,0,0 }, (GXvec3_t) { 1,1,1 });
	while (running)
	{
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					running = 0;
			case SDL_MOUSEMOTION:
				break;
			default:
				break;
			}
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawScene(scene);

		SDL_GL_SwapWindow(window);
	}

	// SDL Deinitialization
	{
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
	// GX Unloading;
	{
		destroyScene(scene);
	}
	return 0;
}