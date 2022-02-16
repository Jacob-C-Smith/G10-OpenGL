#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/G10.h>
#include <G10/GXtypedef.h>
#include <G10/GXTexture.h>

// Texture constructor
GXTexture_t *load_bmp_image ( const char path[] ); // ✅ Loads a BMP file from path and returns a pointer to GXTexture_t
