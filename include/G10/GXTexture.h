#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXDebug.h>
#include <G10/GXtypedef.h>

// Contains information about a texture
struct GXTexture_s
{
	unsigned int textureID;
	GXsize_t width;
	GXsize_t height;
};
typedef struct GXTexture_s GXTexture_t;

GXTexture_t* createTexture ( );                    // ✅ Creates an empty texture
GXTexture_t* loadTexture   ( const char path[] );  // ✅ Loads a texture from the disk
int          unloadTexture ( GXTexture_t* image ); // ✅ Unloads a texture and all of its contents

