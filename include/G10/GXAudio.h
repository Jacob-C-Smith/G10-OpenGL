#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL_mixer.h>

#include <G10/G10.h>
#include <G10/GXtypedef.h>

/*
 * Comment
*/

//Create structs for Sample
//Contains information about a texture
struct GXSample_s
{
    //Name
    char* name;

    //Sample
    Mix_Chunk* sample;

    struct GXSample_s* next;
};

struct GXMusic_s
{
    //Name 
    char* name;

    //Sample
    Mix_Music* sample;

    struct GXMusic_s* next;
};

// Allocators
GXSample_t *create_sample  ( void );
GXMusic_t  *create_music   ( void );

// Constructors
GXSample_t *load_sample    ( const char path[] );
GXMusic_t  *load_music     ( const char path[] );

// Audio control?
int         play_sample    ( GXSample_t *sample );
int         play_music     ( GXMusic_t  *music  );

int         fade_sample    ( GXSample_t *sample, float start_volume, float end_volume, float time );
int         fade_music     ( GXMusic_t  *music , float start_volume, float end_volume, float time );

// Destructor
GXSample_t *destroy_sample ( GXSample_t *sample );
GXMusic_t  *destroy_music  ( GXMusic_t  *music );
