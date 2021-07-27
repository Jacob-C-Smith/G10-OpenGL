#include <G10/GXHDR.h>

GXTexture_t* loadHDR(const char path[])
{
	// Argument checking
	{
		#ifndef NDEBUG
			if(path==0)
				goto noPath;
		#endif
	}

    // Uninitialized data
    u8          *data;
    char        *cData;

    // Initialized data
    GXTexture_t *ret    = createTexture();
    FILE        *f      = fopen(path, "rb");
    size_t       format = 0,
                 x      = 0,
                 y      = 0,
                 i      = 0;

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
        data = calloc(i+1,sizeof(u8));

        // Check if data is valid
        if (data == 0)
            return (void*)0;

        // Read to data
        fread(data, 1, i, f);

        // We no longer need the file
        fclose(f);
    }

    cData = data;

    if (strncmp(cData, "#?RADIANCE",10) != 0)
        goto badFormat;

    cData = 1+strchr(cData-1, '\n');

    // Parse the HDR
    while ((x == 0 && y == 0 && format == 0))
    {
        switch(*cData)
        {
            case '#':
                goto nextLine;
                break;
            case 'F':
            {
                cData += 7;
                if (strncmp(cData, "32-bit_rle_rgbe", 14) != 0)
                    goto badFormat;
                goto nextLine;
            }
            break;
            case '-':
            {
                cData += 3;
                x = atoi(cData);
                cData = 4+strchr(cData, ' ');
                y = atoi(cData);
                goto nextLine;
            }
            break;
            case '\n':
                cData++;
                continue;
                

        }
        nextLine:
        cData = 1+strchr(cData, '\n');
    }

    ret->width  = x;
    ret->height = y;
    ret->name   = "skybox";

    // Set up the OpenGL texture 
    {
        // Create a texture ID
        glGenTextures(1, &ret->textureID);
        glBindTexture(GL_TEXTURE_2D, ret->textureID);

        // Point it to the right place
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ret->width, ret->height, 0, GL_RGB, GL_FLOAT, cData);

        // Bilinear texture filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }

    // Free the file 
    free(data);

    // Debugger logging
    #ifndef NDEBUG
        gPrintLog("[G10] [HDR] Loaded file \"%s\"\n\n", path);
    #endif 

    return ret;

	// Error handling
	{
		noPath:
		#ifndef NDEBUG
			gPrintError("[G10] [HDR] No path provided for parameter \"path\" in function \"%s\"\n", __FUNCSIG__);
		#endif
		return 0;

        invalidFile:
        #ifndef NDEBUG
            gPrintError("[G10] [HDR] Could not open file \"%s\"\n",path);
        #endif
        return 0;

        badFormat:
        #ifndef NDEBUG
            gPrintError("[G10] [HDR] Bad format in file \"%s\"\n", path);
        #endif
        return 0;
	}
}
