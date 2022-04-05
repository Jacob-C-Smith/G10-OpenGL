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
#include <G10/GXSplashscreen.h>
#include <G10/GXScene.h>
#include <G10/GXInput.h>
#include <G10/GXMaterial.h>
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
	SDL_GLContext  gl_context;
	SDL_Event      event;

	// G10 data
	GXInput_t     *input;

	// Loaded scenes
	GXScene_t     *scenes;

	// Server 
	GXServer_t    *server;

	// Cached assets
	GXPart_t     **cached_parts;
	GXMaterial_t **cached_materials;
	GXShader_t   **cached_shaders;

		// Names of cached assets
	char	     **cached_part_names,
		         **cached_material_names,
		         **cached_shader_names;

	// Length of cached asset lists
	size_t	       cached_part_count,
		           cached_material_count,
		           cached_shader_count;

	// Delta time
	u32            d, 
		           lastTime;
	float          delta_time;
    bool           running;

};

// Global initializers
GXInstance_t *g_init                ( const char          *path );                                              // ✅ g_init initializes SDL and OpenGL
 
// File operations
size_t        g_load_file           ( const char          *path    , void         *buffer  , bool binaryMode ); // ✅ Loads a file and reads it into buffer. If buffer is null, function will return size of file, else returns bytes written.

// Debug loggins
int           g_print_error         ( const char *const    format  , ... );                                     // ✅ printf, but in red
int           g_print_warning       ( const char *const    format  , ... );                                     // ✅ printf, but in yellow
int           g_print_log           ( const char *const    format  , ... );                                     // ✅ printf, but in blue

// Window operations
int           g_clear               ( void );                                                                   // ✅ Clears the color and depth buffers
int           g_swap                ( GXInstance_t        *instance );                                          // ✅ Swap buffers
int           g_window_resize       ( GXInstance_t        *instance );                                          // ✅ Responds to window resizes

int           g_delta               ( GXInstance_t        *instance );                                          // ✅ Calculates delta time

void          g_toggle_mouse_lock   ( callback_parameter_t state,    GXInstance_t *instance );                  // ✅ Toggle mouse locking
void          g_toggle_full_screen  ( callback_parameter_t state,    GXInstance_t *instance );                  // ✅ Toggle full screen
int           g_exit_game_loop      ( callback_parameter_t state,    GXInstance_t *instance );                  // ✅ Called for exit

// Getters
GXInstance_t *g_get_active_instance ( void );                                                                   // ✅ Returns a pointer to the active instance

// Cache operations
int           g_cache_material      ( GXInstance_t        *instance, GXMaterial_t *material );                  // ✅ Caches a material for later use
int           g_cache_part          ( GXInstance_t        *instance, GXPart_t     *part );                      // ✅ Caches a part for later use
int           g_cache_shader        ( GXInstance_t        *instance, GXShader_t   *shader );                    // ✅ Caches a shader for later use

GXMaterial_t *g_find_material       ( GXInstance_t        *instance, char         *name );                      // ✅ Searches the cache for a specified material
GXPart_t     *g_find_part           ( GXInstance_t        *instance, char         *name );                      // ✅ Searches the cache for a specified part
GXShader_t   *g_find_shader         ( GXInstance_t        *instance, char         *name );                      // ✅ Searches the cache for a specified shader

// Destructors
int           g_exit                ( GXInstance_t        *instance );                                          // ✅ Deinitializes SDL and OpenGL