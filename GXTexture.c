#include <G10/GXTexture.h>

#include <G10/G10.h>
#include <G10/GXBitmap.h>
#include <G10/GXJPG.h>
#include <G10/GXPNG.h>

GXTextureUnit_t* activeTextures;

GXTexture_t *create_texture                ( )
{
	GXTexture_t* ret = calloc(1,sizeof(GXTexture_t));

    // Check the memory
    #ifndef NDEBUG
        if (ret == 0)
            goto noMem;
    #endif

	ret->texture_unit_index = -1; // Textures start out not being mapped

    return ret;

    // Error handling
    {
        noMem:
        #ifndef NDEBUG
            g_print_error("[G10] [Texture] Out of memory.\n");
        #endif
        return 0;
    }
	
	return ret;
}

GXTexture_t *load_texture                  ( const char   path[] )
{
	// TODO: Argument check
	// Initialized data
	char*        fileExtension = 1+strrchr(path, '.');
	GXTexture_t* ret           = (void*)0;

	// TODO: Find a better way of doing this
	// Figure out what type of file we are dealing with throught the extenstion. This is an admittedly naive approach,
	// but each loader function checks for signatures, so any error handling is handed off to them. 
	if (strcmp(fileExtension, "png") == 0 || strcmp(fileExtension, "PNG") == 0)
		return load_png_image(path); 
	else if (strcmp(fileExtension, "bmp") == 0 || strcmp(fileExtension, "dib") == 0 ||
		     strcmp(fileExtension, "BMP") == 0 || strcmp(fileExtension, "DIB") == 0)
		return load_bmp_image(path);
	else if (strcmp(fileExtension, "jpg")  == 0 || strcmp(fileExtension, "jpeg") == 0 ||
		     strcmp(fileExtension, "jpe")  == 0 || strcmp(fileExtension, "jif")  == 0 ||
	  	     strcmp(fileExtension, "jfif") == 0 || strcmp(fileExtension, "jfi")  == 0 ||
		     strcmp(fileExtension, "JPG")  == 0 || strcmp(fileExtension, "JPEG") == 0 ||
		     strcmp(fileExtension, "JPE")  == 0 || strcmp(fileExtension, "JIF")  == 0 ||
		     strcmp(fileExtension, "JFIF") == 0 || strcmp(fileExtension, "JFI")  == 0)
		return load_jpg_image(path);
	else
	#ifndef NDEBUG
		g_print_error("[G10] [Texture] Could not load file %s, unrecognized file extension.\n", path);
	#endif
	;

	return ret;
	// TODO: Error handling

}

unsigned int bind_texture_to_unit     ( GXTexture_t *image )
{
	// Argument checking
	{
		#ifndef NDEBUG
			if(image==0)
				goto noImage;
		#endif
	}
	
	// TODO: Make atomic for multithreading
	// TODO: Make multithreaded
	// TODO: Partition an even number of texture units per each thread.
	// Iterate over all active textures

	if(image->texture_unit_index != -1)
		return image->texture_unit_index;

	for (size_t i = 0; i < activeTextures->active_texture_count; i++)
	{
		// Check if the active texture unit in the block is not allocated
		if (activeTextures->active_texture_block[i] == 0)
		{
			// Bind the texture to the unused texture unit
			(GXTexture_t*)activeTextures->active_texture_block[i]    = image;                         // Set the texture unit index in the texture
			image->texture_unit_index                                = i;                             // Set the texture unit index
			glActiveTexture(GL_TEXTURE0 + activeTextures->active_texture_block[i]->texture_unit_index); // Set the active texture unit
			glBindTexture((image->cubemap) ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, activeTextures->active_texture_block[i]->texture_id);         // Set the active texture to the corresponding unit
			remove_texture_from_texture_unit((i + (activeTextures->active_texture_count/2)) % activeTextures->active_texture_count);                 // Remove next texture 
			return i;
		}
		else {
			
		}
	}

	return -1;
	
	// Error handling
	{
		noImage:
		#ifndef NDEBUG
			g_print_error("[G10] [Texture] Null parameter provided for \"image\" in function \"%s\"\n",__FUNCSIG__);
		#endif
		return 0;
	}

}

unsigned int remove_texture_from_texture_unit ( unsigned int       index )
{
	if (index == -1)
		return 0;
	activeTextures->active_texture_block[index] = 0;
	return 0;
}

int          unload_texture                ( GXTexture_t *image )
{
	// TODO: Argument check
	// Invalidate width, height.
	image->width  = 0,
	image->height = 0;

	// Delete OpenGL buffers
	glDeleteTextures(1, &image->texture_id);
	image->texture_id = 0;

	// Free the image
	free(image);

	return 0;
	// TODO: Error handling

}
