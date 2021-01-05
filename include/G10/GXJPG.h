#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL2/SDL_image.h>

#include <glad/glad.h>

#include <G10/GXDebug.h>
#include <G10/GXtypedef.h>
#include <G10/GXTexture.h>

GXTexture_t* loadJPGImage ( const char path[] ); // ✅ Loads a JPG file from path and returns a pointer to GXTexture_t
