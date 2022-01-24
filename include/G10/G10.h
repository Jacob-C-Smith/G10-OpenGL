#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>

#include <glad/glad.h>

#include <JSON/JSON.h>

#include <G10/GXtypedef.h>
#include <G10/GXInput.h>
#include <G10/GXTexture.h>

#undef main

struct GXInstance_s
{
	// Name 
	char          *name; 

	// SDL data
	SDL_Window    *window;
	SDL_GLContext  glContext;
	SDL_Event      event;

	// G10 data
	GXInput_t     *input;
	GXTexture_t  **loaded_textures;
	GXPart_t     **loaded_parts;
	GXMaterial_t **loaded_materials;
	GXEntity_t   **loaded_entities;

	char         **loaded_texture_names,
		         **loaded_part_names,
		         **loaded_material_names,
		         **loaded_entitie_names;
	// Delta time
	u32            d,
                   currentTime;
	float          deltaTime;
    bool           running;
    u32	           lastTime;

};

GXInstance_t *g_init          ( const char       *path );                                      // ✅ g_init initializes SDL and OpenGL

size_t        g_load_file     ( const char       *path    , void   *buffer, bool binaryMode ); // ✅ Loads a file and reads it into buffer. If buffer is null, function will return size of file, else returns bytes written.

int           g_print_error   ( const char *const format  , ... );                             // ✅ printf, but in red
int           g_print_warning ( const char *const format  , ... );                             // ✅ printf, but in yellow
int           g_print_log     ( const char *const format  , ... );                             // ✅ printf, but in blue

int           g_clear         ( void );                                                        // ✅ g_clear clears the color and depth buffers
int           g_swap          ( GXInstance_t     *instance );
int           g_delta         ( GXInstance_t     *instance );
u8            g_checksum      ( u8               *data    , size_t len );                      // TODO: Document

int           g_exit          ( GXInstance_t     *instance );                                  // ✅ g_exit deinitializes SDL and OpenGL