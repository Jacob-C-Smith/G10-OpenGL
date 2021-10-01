#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int  createSplashscreen  ( const char *forePath, const char *backPath );
void moveFront           ( int         x,        int         y );
void renderTextures      ( );
int  destroySplashscreen ( );