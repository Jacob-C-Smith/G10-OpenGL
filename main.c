#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <SDL2/SDL.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>

#include <G10/GXScene.h>
#include <G10/GXCamera.h>

#include <G10/GXEntity.h>

#include <G10/GXRigidbody.h>

#include <time.h>

#ifdef _M_X64
#include <G10/arch/x86_64/GXSSEmath.h>
#endif

#include <G10/G10.h>

#undef main

int main(int argc, const char* argv[])
{
	// Uninitialized G10 data
	GXScene_t*    scene;
	u32           d,
		          currentTime;

	// Uninitialized SDL data
	SDL_Window*   window;
	SDL_GLContext glContext;
	SDL_Event     event;

	// Initialized Data
	float         deltaTime = 0.f;
	u8            running   = 1;
	u32	          lastTime  = 0;

	// Parse command line arguments
	{
		for (size_t i = 1; i < argc; i++)
		{
			if (strcmp(argv[i], "-help") == 0)
			{
				// Initialize SDL
				if (SDL_Init(SDL_INIT_EVERYTHING))
				{
					printf("Failed to initialize SDL\n");
					return -1;
				}
				SDL_OpenURL("help.html");
				return 0;
			}
		}
	}

	// G10 Initialization Junk
	{
		gInit(&window, &glContext);
		scene = loadScene("gameassets/scene.json");
	}

	// Main game loop
	while (running)
	{
		// Calculate delta time
		{
			currentTime = SDL_GetTicks();
			d = currentTime - lastTime;
			lastTime = currentTime;
			deltaTime = (float)1 / d;
		}

		printf("%g\r", deltaTime * 1000); // Uses CR instead of CR LF to provide a (kind of) realtime readout of the FPS


		// TODO: Find a better way to process input.
		// Process input
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_1:
					setActiveCamera(scene, "Primary Camera");
					break;
				case SDLK_2:
					setActiveCamera(scene, "Secondary Camera");
					break;
				case SDLK_3:
					setActiveCamera(scene, "Tertiary Camera");
					break;
				case SDLK_ESCAPE:
					running = 0;
					break;
				default:
					break;
				}
			}
			break;
			case SDL_MOUSEMOTION:
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				{
					// Respond to window resizing
					int w = 0, h = 0;

					// Pull window data
					SDL_GetWindowSize(window, &w, &h);
					scene->cameras->fov = (float)w / h;

					// Notify OpenGL of the change
					glViewport(0, 0, w, h);
					break;
				}
				default:
					break;
				}
			default:
				break;
			}
		}

		// Clear the screen
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute physics
		//computePhysics(scene, deltaTime);

		// Draw the scene
		drawScene(scene);

		// Swap the window 
		SDL_GL_SwapWindow(window);

	}

	// GX Unloading;
	{
		destroyScene(scene);
		gExit(window, glContext, event);
	}

	return 0;
}
