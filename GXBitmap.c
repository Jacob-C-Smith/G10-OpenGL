#include <G10/GXBitmap.h>

GXTexture_t *load_bmp_image ( const char path[] ) 
{
    // Argument check
    {
        #ifndef NDEBUG
        if (path == 0)
            goto noPath;
        #endif
    }

    // Uninitialized data
    u8          *data;
    size_t       i;
    
    // Initialized data
    GXTexture_t *ret = create_texture();
    FILE        *f   = fopen(path, "rb");

    // Check allocated memory
    #ifndef NDEBUG
        if (ret == 0)
            return ret;
    #endif

    // Load up the file
    i    = g_load_file(path, 0, true);
    data = calloc(i, sizeof(u8));
    
    // Check  allocated memory
    #ifndef NDEBUG
        if (data == (void*)0)
            return 0;
    #endif
    
    // Load the file
    g_load_file(path, data, true);

    // Fill out width and height information
    ret->width  = *(size_t*)&data[0x12];
    ret->height = *(size_t*)&data[0x16];

    // Set up the OpenGL texture 
    {
        // Create a texture ID
        glGenTextures(1, &ret->texture_id);
        glBindTexture(GL_TEXTURE_2D, ret->texture_id);

        // Point it to the right place
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)ret->width, (int)ret->height, 0, GL_BGR, GL_UNSIGNED_BYTE, &data[0x36]);

        // Bilinear texture filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // Create mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Free data. We don't really need the header anymore. 
    free(data);

    // Debugger logging
    #ifndef NDEBUG
        g_print_log("[G10] [BMP] Loaded file \"%s\"\n\n", path);
    #endif 

    return ret;

    // Error handling
    {
        // No path supplied
        noPath:
            #ifndef NDEBUG
                g_print_error("[G10] [BMP] No path supplied to \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
    }
}