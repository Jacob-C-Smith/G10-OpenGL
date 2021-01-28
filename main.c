#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>

#include <G10/GXScene.h>
#include <G10/GXCamera.h>

#include <G10/GXEntity.h>

#undef main

int main( int argc, const char* argv[] )
{
	// Uninitialized SDL data
	SDL_Window*   window;
	SDL_Event     event;
	SDL_GLContext glContext;
	
	// Uninitialized G10 data
	GXScene_t*    scene;

	// Initialized Data
	u8            running                  = 1;
	u32           d, currentTime, lastTime = 0;
	
	GXvec3_t a = addVec3((GXvec3_t) { 6.f, 6.f, 6.f }, (GXvec3_t) { 1.f, 1.f, 1.f });

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
		window = SDL_CreateWindow( "G10",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			1280,
			720,
			/*1920,
			1080,*/
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );


		
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
		// Enable VSync
		SDL_GL_SetSwapInterval(1);
	
		// Uncomment for mouse locking
		//SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	// G10 Initialization Junk
	{
		scene = loadScene("gameassets/scene.json");
		scene->head->rigidbody = createRigidBody(0.1f, true);
	}


	// OpenGL Commands
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	// Main game loop
	while (running)
	{
		// Calculate delta time
		currentTime = SDL_GetTicks();
		d           = currentTime - lastTime;
		lastTime    = currentTime;

		printf("%d\n", d);

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
						setActiveCamera(scene, "Ternary Camera");
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
					float newFOV;

					// Pull window data
					SDL_GetWindowSize(window, &w, &h);
					scene->camera->fov = (float)w / h;

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
		
		// Process physics
		
		//updatePositionAndVelocity(scene->head->rigidbody, scene->head->transform, d);

		// Draw the scene
		drawScene(scene);
		SDL_Delay(1);

		// Swap the window 
		SDL_GL_SwapWindow(window);
		
	}
	
	// GX Unloading;
	{
		destroyScene(scene);
	}

	// SDL Deinitialization
	{
		SDL_DestroyWindow(window);
		SDL_GL_DeleteContext(glContext);
		IMG_Quit();
		SDL_Quit();
	}

	return 0;
}