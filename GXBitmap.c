#include <G10/GXBitmap.h>

GXtexture_t* loadBMPImage(const char path[])
{
	// Uninitialized data
	int          i;
	u8*          data;

	// Initialized data
	GXtexture_t* ret = malloc(sizeof(GXtexture_t));
	FILE*        f   = fopen(path, "rb");

	// Check allocated memory
	if (ret == 0)
		return (void*)0;

	// Check if file is valid
	if (f == NULL)
	{
		printf("Failed to load file %s\n", path);
		return (void*)0;
	}	

	// Find file size and prep for read
	fseek(f, 0, SEEK_END);
	i = ftell(f);
	fseek(f, 0, SEEK_SET);

	// Allocate data and read file into memory
	data = malloc(i);

	// Check if data is valid
	if (data == 0)
		return (void*)0;

	// Read to data
	fread(data, 1, i, f);

	// We no longer need the file
	fclose(f);

	// Fill out width and height information
	ret->width  = *(GXsize_t*)&data[0x12];
	ret->height = *(GXsize_t*)&data[0x16];

	// Debugger logging
	#ifdef debugmode
		printf("Loaded file %s\nWidth  %i px\nHeight %i px\n\n", path, ret->width, ret->height);
	#endif 

	// Create a texture ID
	glGenTextures(1, &ret->textureID);
	glBindTexture(GL_TEXTURE_2D, ret->textureID);

	// Point it to the right place
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ret->width, ret->height, 0, GL_BGR, GL_UNSIGNED_BYTE, &data[0x36]);
	
	// More OpenGL things
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);

	// Free data. We don't really need the header anymore. 
	free(data);

	return ret;
}