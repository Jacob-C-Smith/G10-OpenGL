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
	{
		#ifndef NDEBUG
		    if (ret == 0)
	            goto noMem;
		#endif
	}

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

	// Argument check
	{
		if (path == (void*)0)
			goto noPath;
	}

	// Initialized data
	char*        fileExtension = 1+strrchr(path, '.');
	GXTexture_t* ret           = (void*)0;

	// TODO: It might be worth finding a better way of doing this?
	
	// Figure out the file type via the extension. This is an admittedly naive approach, 
	// but each loader function checks for signatures, so any error handling is handed off to them. 
	
	// Load as a png
	if      ( strcmp( fileExtension, "png" ) == 0 || strcmp( fileExtension, "PNG" ) == 0 )

		return load_png_image(path); 

	// Load as a bitmap
	else if ( strcmp( fileExtension, "bmp" ) == 0 || strcmp( fileExtension, "dib" ) == 0 ||
		      strcmp( fileExtension, "BMP" ) == 0 || strcmp( fileExtension, "DIB" ) == 0 )

		return load_bmp_image(path);
	// Load as a jpeg
	else if (strcmp( fileExtension, "jpg" )  == 0 || strcmp( fileExtension, "jpeg" ) == 0 ||
		     strcmp( fileExtension, "jpe" )  == 0 || strcmp( fileExtension, "jif" )  == 0 ||
	  	     strcmp( fileExtension, "jfif" ) == 0 || strcmp( fileExtension, "jfi" )  == 0 ||
		     strcmp( fileExtension, "JPG" )  == 0 || strcmp( fileExtension, "JPEG" ) == 0 ||
		     strcmp( fileExtension, "JPE" )  == 0 || strcmp( fileExtension, "JIF" )  == 0 ||
		     strcmp( fileExtension, "JFIF" ) == 0 || strcmp( fileExtension, "JFI" )  == 0 )

		return load_jpg_image(path);
	else if (strcmp( fileExtension, "hdr")   == 0 || strcmp( fileExtension, "HDR")   == 0 )

		return load_hdr(path);
	else
		#ifndef NDEBUG
			g_print_error("[G10] [Texture] Could not load file %s, \"%s\" is an unrecognized file extension.\n", path, fileExtension);
		#endif
		;

	return ret;

	// Error handling
	{

		// Argument errors
		{
			noPath:
				#ifndef NDEBUG
					g_print_error("[G10] [Texture] Null pointer provided for \"path\" in call to %s\n", __FUNCSIG__);
				#endif	
			return 0;
		}
	}

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
	
	// TODO: Make atomic for multithreading?
	//       Make multithreaded?
	//       Partition an even number of texture units per each thread?
	
	// If the texture is not unmapped, it must be mapped
	if(image->texture_unit_index != -1)
		return image->texture_unit_index;

	// Find a texture slot
	// Iterate over all active textures
	for (unsigned int i = 0; i < activeTextures->active_texture_count; i++)
	{
		// Check if the active texture unit in the block is not allocated
		if (activeTextures->active_texture_block[i] == 0)
		{
			// Bind the texture to the unused texture unit
			{
				// Set the index in the texture
				(GXTexture_t*)activeTextures->active_texture_block[i] = image;

				// Set the texture unit index
				image->texture_unit_index = i;

				// Activate and bind the texture
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture((image->cubemap == true) ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, image->texture_id);

				// Remove next texture 
				remove_texture_from_texture_unit((i + 32) % activeTextures->active_texture_count);

				return i;
			}
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
	if (index == -1 || index > activeTextures->active_texture_count)
		return 0;
	activeTextures->active_texture_block[index] = 0;
	return 0;
}

int          unload_texture                ( GXTexture_t *image )
{
	// Argument check
	{
		if (image == (void*)0)
			goto no_image;
	}

	// Delete OpenGL buffers
	glDeleteTextures(1, &image->texture_id);
	image->texture_id = 0;

	// Free the image
	free(image);

	return 0;

	// Error handling
	{

		// Argument errors
		{
			no_image:
			#ifndef NDEBUG
				g_print_error("[G10] [Texture] Null pointer provided for \"image\" in call to \"%s\"\n", __FUNCSIG__);
			#endif
			return 0;
		}
	}

}
