#include <G10/GXSplashscreen.h>

SDL_Window   *window;
SDL_Renderer *renderer;
SDL_Texture  *img;

int createSplashscreen ( const char* imagePath )
{
	// Uninitialized data
	unsigned int w,
	             h;
	SDL_Rect     texr;

	// Create a new window and renderer
	window   = SDL_CreateWindow("G10 Splashscreen", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 512, SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Load up the specified texture and get the width and height
	img = IMG_LoadTexture(renderer, imagePath);
	SDL_QueryTexture(img, NULL, NULL, &w, &h); 
	texr.x = 0; texr.y = 0; texr.w = w; texr.h = h;

	// Clear the screen
	SDL_RenderClear(renderer);

	// Copy the texture to the rendering context
	SDL_RenderCopy(renderer, img, NULL, &texr);
	
	// Draw to the screen
	SDL_RenderPresent(renderer);
}

int destroySplashscreen()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(img);
}