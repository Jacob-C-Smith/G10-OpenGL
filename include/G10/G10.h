#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>

#include <glad/glad.h>

#include <G10/GXTexture.h>

int    gInit         ( SDL_Window      **window, SDL_GLContext *glContext ); // ✅ gInit initializes SDL and OpenGL

size_t gLoadFile     ( const char       *path, void            *buffer );    // ✅ Loads a file and reads it into buffer. If buffer is null, function will return size of file, else returns bytes written.

int    gPrintError   ( const char *const format, ... );                      // ✅ gPrintError is printf, but in red
int    gPrintWarning ( const char *const format, ... );                      // ✅ gPrintWarning is printf, but in yellow
int    gPrintLog     ( const char *const format, ... );                      // ✅ gPrintLog is printf, but in blue

int    gExit         ( SDL_Window       *window, SDL_GLContext  glContext ); // ✅ gExit deinitializes SDL and OpenGL