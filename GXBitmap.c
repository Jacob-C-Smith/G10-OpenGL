#include <G10/GXBitmap.h>

GXTexture_t* loadBMPImage ( const char path[] )
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
    int          i;
    
    // Initialized data
    GXTexture_t *ret = createTexture();
    FILE        *f   = fopen(path, "rb");

    // Check allocated memory
    if (ret == 0)
        return ret;

    // Load the file
    {
        // Check if file is valid
        if (f == NULL)
            goto invalidFile;

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

    // Set up the OpenGL texture 
    {
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
    }

    // Free data. We don't really need the header anymore. 
    free(data);

    // Debugger logging
    #ifndef NDEBUG
        printf("[G10] [BMP] Loaded file \"%s\"\n\n", path);
    #endif 

    return ret;

    // Error handling
    {
        // No path supplied
        noPath:
            #ifndef NDEBUG
                printf("[G10] [BMP] No path supplied to \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;

        // Invalid file
        invalidFile:
            #ifndef NDEBUG
                printf("[G10] [BMP] Failed to load file %s\n", path);
            #endif
            return 0;
    }
}