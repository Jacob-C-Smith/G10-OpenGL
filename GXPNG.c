#include <G10/GXPNG.h>

GXTexture_t *load_png_image ( const char path[] )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if(path == 0)
                goto noPath;
        #endif
    }

    // Uninitialized data
    SDL_Surface *image;
    u8          *data;
    size_t       allocateSize;
    

    // Initialized data
    GXTexture_t *ret = create_texture();
    SDL_RWops   *r   = SDL_RWFromFile(path, "rb");
    
    // Check allocated memory
    if (ret == 0)
        return ret;

    // Load the PNG through SDL
    image = IMG_LoadPNG_RW(r);
    if (!image)
        goto invalidFile;

    // Copy some information
    allocateSize = (size_t)image->w * (size_t)image->h * (size_t)image->format->BytesPerPixel;
    ret->width   = image->w;
    ret->height  = image->h;

    // Allocate space for the pixel data
    data         = calloc(allocateSize+1,sizeof(u8));

    if (data == 0)
        return (void*)0;

    // Copy PNG data
    memcpy(data,image->pixels, allocateSize);

    // Set up the OpenGL texture 
    {
        // Create a texture ID
        glGenTextures(1, &ret->texture_id);
        glBindTexture(GL_TEXTURE_2D, ret->texture_id);

        // Point it to the right place, use some ternarry operaters to determine color depth
        glTexImage2D(GL_TEXTURE_2D, 0, (image->format->BytesPerPixel == 1) ? GL_RED : ((image->format->BytesPerPixel == 3) ? GL_RGB : GL_RGBA), ret->width, ret->height, 0, (image->format->BytesPerPixel == 1) ? GL_RED : (image->format->BytesPerPixel == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
        
        // Mip maps
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Free data.
    free(data);
    SDL_FreeSurface(image);
    SDL_FreeRW(r);

    // Debugger logging
    #ifndef NDEBUG
        g_print_log("[G10] [Texture] [PNG] Loaded file \"%s\"\n", path);
    #endif 
    
    return ret;

    // Error handling
    {
        invalidFile:
        #ifndef NDEBUG
            g_print_error("[G10] [Texture] [PNG] Failed to load file %s\n", path);
        #endif
        return 0;

        noPath:
        #ifndef NDEBUG
            g_print_error("[G10] [Texture] [PNG] No path provided in call to funciton \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}