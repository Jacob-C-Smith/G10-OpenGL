#include <G10/GXPNG.h>

GXtexture_t* loadPNGImage( const char path[] )
{
	// Uninitialized data
	SDL_Surface* image;
	size_t       allocateSize;
	u8*          data;

	// Initialized data
	GXtexture_t* ret = malloc(sizeof(GXtexture_t));
	SDL_RWops*   r   = SDL_RWFromFile(path, "rb");
	
	// Check allocated memory
	if (ret == 0)
		return (void*)0;

	// Load the PNG through SDL
	image = IMG_LoadPNG_RW(r);
	if (!image)
	{
		printf("Failed to load file %s\n", path);
		return (void*)0;
	}

	// Copy some information
	allocateSize = (size_t)image->w * (size_t)image->h * (size_t)image->format->BytesPerPixel;
	ret->width   = image->w;
	ret->height  = image->h;

	// Allocate space for the pixel data
	data         = malloc(allocateSize);

	// Check if data is valid
	if (data == 0)
		return (void*)0;

	// Copy what we need.
	memcpy(data,image->pixels, allocateSize);

	// Create a texture ID
	glGenTextures(1, &ret->textureID);
	glBindTexture(GL_TEXTURE_2D, ret->textureID);

	// Point it to the right place, use some ternarry operaters to determine color depth
	glTexImage2D(GL_TEXTURE_2D, 0, (image->format->BytesPerPixel == 3) ? GL_RGB : GL_RGBA, ret->width, ret->height, 0, (image->format->BytesPerPixel == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);

	// Free data.  
	free(data);
	SDL_FreeSurface(image);
	SDL_FreeRW(r);

	return ret;
}