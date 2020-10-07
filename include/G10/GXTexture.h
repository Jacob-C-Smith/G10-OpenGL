#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXDebug.h>
#include <G10/GXtypedef.h>

// Contains information about a texture
struct GXtexture_s
{
	unsigned int textureID;
	GXsize_t width;
	GXsize_t height;
};
typedef struct GXtexture_s GXtexture_t;

int unloadTexture (GXtexture_t* image); // ✅ Unloads a texture and all of its contents

