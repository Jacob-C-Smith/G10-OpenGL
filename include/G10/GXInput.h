#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include <G10/GXtypedef.h>
#include <G10/G10.h>
#include <JSON/JSON.h>

struct GXBind_s
{
	char     *name,
	   	    **keys;
	void    **callbacks;
	size_t    key_count,
		      callback_count,
		      callback_max;
	GXBind_t *next;
};
struct GXInput_s
{
	char     *name;
	GXBind_t *binds;
};

// Allocators
GXBind_t  *create_bind              ( void );
GXInput_t *create_input             ( void );

// Constructors
GXInput_t *load_input               ( const char path[] );
GXInput_t *load_input_as_json       ( char      *token );
GXBind_t  *load_bind_as_json        ( char      *token );

// Callback registration
int        register_bind_callback   ( GXBind_t *bind   , void *function_pointer );

// Callback removal
int        unregister_bind_callback ( GXBind_t *bind   , void *function_pointer );

// Keycode finder
SDL_KeyCode find_key                ( const char *name );

// Key information
int         print_all_keys          ( void );
int         print_all_binds         ( GXInput_t *inputs );

// Input processing
int         process_input           ( GXInstance_t *instance );

// Bind firing
int         fire_bind               ( GXBind_t *bind );

// Bind operations
int         append_bind             ( GXInput_t *input, GXBind_t *bind );
int         find_bind               ( GXInput_t *input, char     *name );
int         remove_bind             ( GXInput_t *input, GXBind_t *bind );

// Deallocators
int         destroy_bind            ( GXBind_t  *bind );
int         destroy_input           ( GXInput_t *input ); 