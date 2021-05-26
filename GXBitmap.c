#include <G10/GXBitmap.h>

GXTexture_t* loadBMPImage ( const char path[] )
{
	// Uninitialized data
	int          i;
	u8*          data;

	// Initialized data
	GXTexture_t* ret = malloc(sizeof(GXTexture_t));
	FILE*        f   = fopen(path, "rb");

	// Check allocated memory
	if (ret == 0)
		return ret;

	// Load the file
	{
		// Check if file is valid
		if (f == NULL)
		{
			// Error handling
			#ifndef NDEBUG
				printf("Failed to load file %s\n", path);
			#endif 
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
	}

	// Fill out width and height information
	ret->width  = *(size_t*)&data[0x12];
	ret->height = *(size_t*)&data[0x16];

	// Create a texture ID
	glGenTextures(1, &ret->textureID);
	glBindTexture(GL_TEXTURE_2D, ret->textureID);

	// Point it to the right place
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ret->width, ret->height, 0, GL_BGR, GL_UNSIGNED_BYTE, &data[0x36]);
	
	// Bilinear texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Create mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Free data. We don't really need the header anymore. 
	free(data);

	// Debugger logging
	#ifndef NDEBUG
		printf("Loaded file \"%s\"\n\n", path);
	#endif 

	return ret;
}