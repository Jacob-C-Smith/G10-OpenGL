#include <G10/GXTexture.h>

int unloadTexture( GXtexture_t* image )
{
	// Invalidate width, height. Free image.
	image->width  = 0;
	image->height = 0;

	// Delete OpenGL buffers
	glDeleteTextures(1, &image->textureID);
	image->textureID = 0;

	// Free Image memory
	free(image);

	// Return nullptr
	return 0;
}