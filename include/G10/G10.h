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
#include <G10/GXScene.h>
#include <G10/GXInput.h>
#include <G10/GXTexture.h>

#undef main

struct GXInstance_s
{
	// Name 
	char          *name; 
	
	// Ticks
	size_t         ticks;

	// SDL data
	SDL_Window    *window;
	SDL_GLContext  glContext;
	SDL_Event      event;

	// G10 data
	GXInput_t     *input;

	// Loaded scenes
	GXScene_t     *scenes;

	// Server 
	GXServer_t   *server;

	// Cached assets
	GXTexture_t  **cached_textures;
	GXPart_t     **cached_parts;
	GXMaterial_t **cached_materials;
	GXEntity_t   **cached_entities;

	// Names of cached assets
	char         **cached_texture_names,
		         **cached_part_names,
		         **cached_material_names,
		         **cached_entity_names;

	// Length of cached asset lists
	size_t         cached_texture_count,
		           cached_part_count,
		           cached_material_count,
				   cached_entity_count;

	// Delta time
	u32            d, 
		           lastTime;
	float          delta_time;
    bool           running;

};

GXInstance_t *g_init                ( const char           *path );                                      // ✅ g_init initializes SDL and OpenGL
 
size_t        g_load_file           ( const char           *path    , void   *buffer, bool binaryMode ); // ✅ Loads a file and reads it into buffer. If buffer is null, function will return size of file, else returns bytes written.

int           g_print_error         ( const char *const     format  , ... );                             // ✅ printf, but in red
int           g_print_warning       ( const char *const     format  , ... );                             // ✅ printf, but in yellow
int           g_print_log           ( const char *const     format  , ... );                             // ✅ printf, but in blue

int           g_clear               ( void );                                                        // ✅ g_clear clears the color and depth buffers
int           g_swap                ( GXInstance_t         *instance );
int           g_window_resize       ( GXInstance_t         *instance );
int           g_exit_game_loop      ( callback_parameter_t  c       , GXInstance_t* i );

GXInstance_t *g_get_active_instance ( void );

int           g_cache_texture       ( GXInstance_t         *instance, GXMaterial_t *material );
int           g_cache_material      ( GXInstance_t         *instance, GXMaterial_t *material );
int           g_cache_part          ( GXInstance_t         *instance, GXPart_t     *part );
int           g_cache_entity        ( GXInstance_t         *instance, GXMaterial_t *material );

GXMaterial_t *g_find_material       ( GXInstance_t         *instance, char *name );
GXPart_t     *g_find_part           ( GXInstance_t         *instance, char *name );

int           g_delta               ( GXInstance_t         *instance );

void          g_toggle_mouse_lock   ( void );

int           g_exit                ( GXInstance_t         *instance );                                  // ✅ g_exit deinitializes SDL and OpenGL