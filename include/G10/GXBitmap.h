#pragma once
#include <G10/GXDebug.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>
#include <G10/GXTexture.h>

GXtexture_t* loadBMPImage (const char path[]); // ✅ Loads a BMP file from path and returns a pointer to GXtexture_t
