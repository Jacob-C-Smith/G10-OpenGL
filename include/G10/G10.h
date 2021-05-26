#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glad/glad.h>

int gInit(SDL_Window** window, SDL_GLContext* glContext); // gInit initializes SDL and OpenGL
int gExit(SDL_Window*  window, SDL_GLContext  glContext); // gExit deinitializes SDL and OpenGL