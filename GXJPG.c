#include <G10/GXJPG.h>

GXTexture_t* load_jpg_image ( const char path[] )
{
    // Argument check
    {
        if (path == (void*)0)
            goto no_path;
    }

    // Uninitialized data
    SDL_Surface* image;
    size_t       allocateSize;
    u8*          data;

    // Initialized data
    GXTexture_t* ret = create_texture();
    SDL_RWops*   r   = SDL_RWFromFile(path, "rb");

    // Check allocated memory
    if (ret == 0)
        return ret;

    // Load file through SDL
    image = IMG_LoadJPG_RW(r);

    // Check file
    if (!image)
        goto invalidFile;

    // Copy some information
    allocateSize = (size_t)image->w * (size_t)image->h * (size_t)image->format->BytesPerPixel;
    ret->width   = image->w;
    ret->height  = image->h;

    // Allocate space for the pixel data
    data = calloc(allocateSize,sizeof(u8));
    if (data == 0)
        return (void*)0;

    // Copy the required data
    memcpy(data, image->pixels, allocateSize);

    // Set up the OpenGL texture 
    {
        // Create a texture ID
        glGenTextures(1, &ret->texture_id);
        glBindTexture(GL_TEXTURE_2D, ret->texture_id);

        // Point it to the right place
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ret->width, ret->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        // More OpenGL things
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Free JPEG data
    free(data);
    SDL_FreeSurface(image);
    SDL_FreeRW(r);

    // Debugger logging
    #ifndef NDEBUG
        g_print_log("[G10] [JPG] Loaded file \"%s\"\n\n", path);
    #endif 

    return ret;

    // Error handling
    {
        invalidFile:
        #ifndef NDEBUG
            g_print_error("[G10] Failed to load file %s\n", path);
        #endif
        return 0;

        // Argument errors
        {
            no_path:
            #ifndef NDEBUG
                g_print_error("[G10] [JPG] Null pointer provided for \"path\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}