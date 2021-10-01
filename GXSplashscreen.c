#include <G10/GXSplashscreen.h>

SDL_Window   *window;
SDL_Renderer *renderer;
SDL_Texture  *front;
SDL_Texture  *back; 
SDL_Rect     fTexture,
		     bTexture;

int createSplashscreen ( const char *forePath, const char *backPath )
{
	// Uninitialized data
	unsigned int fw, 
		         bw,
	             fh,
		         bh;
	


	// Create a new window and renderer
	window   = SDL_CreateWindow("G10 Splashscreen", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 512, SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


	// Load up the specified texture and get the width and height
	front = IMG_LoadTexture(renderer, forePath);
	back  = IMG_LoadTexture(renderer, backPath);

	SDL_SetTextureBlendMode(front, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(front, 255);

	SDL_QueryTexture(front, NULL, NULL, &fw, &fh);
	SDL_QueryTexture(back, NULL, NULL, &bw, &bh);

	fTexture.x = 0; fTexture.y = 0; fTexture.w = fw; fTexture.h = fh;
	bTexture.x = 0; bTexture.y = 0; bTexture.w = fw; bTexture.h = fh;

	// Render the splash screen
	renderTextures();

	return 0;
}

void moveFront(int x, int y)
{
	fTexture.x = x; fTexture.y = y;
}

void renderTextures()
{

	// Clear the screen
	SDL_RenderClear(renderer);

	// Copy the texture to the rendering context
	SDL_RenderCopy(renderer, back, NULL, &bTexture);
	SDL_RenderCopy(renderer, front, NULL, &fTexture);

	// Draw to the screen
	SDL_RenderPresent(renderer);
}

int destroySplashscreen()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(front);
	SDL_DestroyTexture(back);
}