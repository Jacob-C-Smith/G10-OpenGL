#include <G10/GXTexture.h>

int unloadTexture(GXtexture_t* image)
{
	// Invalidate width, height. Free image.
	image->width  = 0;
	image->height = 0;

	glDeleteTextures(1, &image->textureID);
	free(image);
	return 0;
}