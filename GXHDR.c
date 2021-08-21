#include <G10/GXHDR.h>

float convertComponent(int expo, int val)
{
    float v = val / 256.0f;
    float d = (float)pow(2, expo);
    return v * d;
}

void rgbeToFloats(RGBE* scan, int len, float* cols)
{
    while (len-- > 0) {
        int expo = scan[0][3] - 128;
        cols[0] = convertComponent(expo, scan[0][0]);
        cols[1] = convertComponent(expo, scan[0][1]);
        cols[2] = convertComponent(expo, scan[0][2]);
        cols += 3;
        scan++;
    }
}


bool decodeScanline(RGBE* scanline, int len, FILE* file)
{
    int i;
    int rshift = 0;

    while (len > 0) {
        scanline[0][0] = fgetc(file);
        scanline[0][1] = fgetc(file);
        scanline[0][2] = fgetc(file);
        scanline[0][3] = fgetc(file);
        if (feof(file))
            return false;

        if (scanline[0][0] == 1 &&
            scanline[0][1] == 1 &&
            scanline[0][2] == 1) {
            for (i = scanline[0][3] << rshift; i > 0; i--) {
                memcpy(&scanline[0][0], &scanline[-1][0], 4);
                scanline++;
                len--;
            }
            rshift += 8;
        }
        else {
            scanline++;
            len--;
            rshift = 0;
        }
    }
    return true;
}

bool decodeRLEScanline(RGBE* scanline, int len, FILE* file)
{
    int  i, j;

    if (len < 8 || len > 0x7FFF)
        return decodeScanline(scanline, len, file);

    i = fgetc(file);
    if (i != 2) {
        fseek(file, -1, SEEK_CUR);
        return decodeScanline(scanline, len, file);
    }

    scanline[0][1] = fgetc(file);
    scanline[0][2] = fgetc(file);
    i = fgetc(file);

    if (scanline[0][1] != 2 || scanline[0][2] & 128) {
        scanline[0][0] = 2;
        scanline[0][3] = i;
        return decodeScanline(scanline + 1, len - 1, file);
    }

    // read each component
    for (i = 0; i < 4; i++) {
        for (j = 0; j < len; ) {
            unsigned char code = fgetc(file);
            if (code > 128) { // run
                code &= 127;
                unsigned char val = fgetc(file);
                while (code--)
                    scanline[j++][i] = val;
            }
            else {	// non-run
                while (code--)
                    scanline[j++][i] = fgetc(file);
            }
        }
    }

    return feof(file) ? false : true;
}





GXTexture_t* loadHDR( const char path[] )
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
                 w      = 0,
                 h      = 0,
                 i      = 0;

    // Check allocated memory
    if (ret == 0)
        return ret;

    // Load up the file
    i = gLoadFile(path, 0);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data);

    cData = data;
    if (strncmp(cData, "#?RADIANCE",10) != 0)
        goto badFormat;

    cData = 1+strchr(cData-1, '\n');

    // Parse the HDR header
    while ((w == 0 && h == 0 && format == 0))
    {
        switch(*cData)
        {
            // This is a comment
            case '#':
                goto nextLine;
                break;
            // This is probably the format
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
                h = atoi(cData);
                cData = 4+strchr(cData, ' ');
                w = atoi(cData);
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

    // Set width and height. Set name to (null)
    ret->width  = w;
    ret->height = h;
    ret->name   = 0;

    float *c = calloc( w * h * 3, sizeof(float));
    float* oc = c;
    RGBE* scanline = calloc(w, sizeof(RGBE));
    fseek(f, cData - data, SEEK_SET);
    
    // convert image 
    for (int y = h - 1; y >= 0; y--) {
        if (decodeRLEScanline(scanline, w, f) == false)
            break;
        rgbeToFloats(scanline, w, c);
        c += w * 3;
    }
    
    c = oc;

    // We no longer need the file
    fclose(f);

    // Set up the OpenGL texture 
    {
        // Create a texture ID
        glGenTextures(1, &ret->textureID);
        glBindTexture(GL_TEXTURE_2D, ret->textureID);

        // Point it to the right place
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, c);

        // Bilinear texture filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }

    // Free the file 
    free(data);
    free(c);

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
