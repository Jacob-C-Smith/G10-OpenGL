#include <G10/G10.h>

int gInit(SDL_Window** window, SDL_GLContext* glContext)
{
	// Uninitialized data
	SDL_Window*    lWindow;
	SDL_GLContext  lGlContext;

	// SDL + GLAD Initialization Junk
	{

		// Initialize SDL
		if (SDL_Init(SDL_INIT_EVERYTHING))
		{
			#ifndef NDEBUG
				printf("Failed to initialize SDL\n");
			#endif
			return 0;
		}

		// Initialize the image loader library
		if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0)
		{
			#ifndef NDEBUG
				printf("Failed to initialize SDL Image\n");
			#endif
			return 0;
		}

		// Create the window
		lWindow = SDL_CreateWindow("G10",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			1280, 720,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE ); 
		
		// Create the OpenGL context
		lGlContext = SDL_GL_CreateContext(lWindow);

		// Check the window
		if (!window)
		{
			#ifndef NDEBUG
				printf("Failed to create a window\nSDL Says: %s", SDL_GetError());
			#endif
			return 0;
		}

		// Check the OpenGL context
		if (!lGlContext)
		{
			#ifndef NDEBUG
				printf("Failed to create an OpenGL Context\nSDL Says: %s", SDL_GetError());
			#endif
			return 0;
		}

		// Check the glad loader
		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{
			#ifndef NDEBUG
				printf("Failed to load OpenGL");
			#endif 

			return 0;
		}

		// Enable VSync
		SDL_GL_SetSwapInterval(1);

		// Uncomment for mouse locking
		SDL_SetRelativeMouseMode(SDL_TRUE);

		// OpenGL Commands
		{
			// Enable depth testing
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			// Set the clear color to white
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	*window    = lWindow;
	*glContext = lGlContext;

	return 0;
}

int gExit(SDL_Window* window, SDL_GLContext glContext)
{
	// SDL Deinitialization
	{
		SDL_DestroyWindow(window);
		SDL_GL_DeleteContext(glContext);
		IMG_Quit();
		SDL_Quit();
	}

	return 0;
}