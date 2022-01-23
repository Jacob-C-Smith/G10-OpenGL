#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int  create_splashscreen  ( const char *forePath, const char *backPath );
void move_front           ( int         x,        int         y );
void render_textures      ( void );
int  destroy_splashscreen ( void );