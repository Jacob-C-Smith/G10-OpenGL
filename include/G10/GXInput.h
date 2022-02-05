#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include <G10/GXtypedef.h>
#include <G10/G10.h>
#include <JSON/JSON.h>

#define UI_M1     0x01
#define UI_M2     0x02
#define UI_M3     0x04
#define UI_M4     0x08
#define UI_M5     0x10
#define UI_SWUP   0x20
#define UI_SWDOWN 0x40

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

enum input_state_e
{
	KEYBOARD = 1,
	MOUSE    = 2
};

struct callback_parameter_s
{
	enum input_state_e input_state;
	union {
		struct {
			bool depressed;
		} key;

		struct mouse_state_s {
			u32 xrel,
				yrel;
			u8  button;
		} mouse_state;
	} inputs;
};

// Allocators
GXBind_t    *create_bind              ( void );
GXInput_t   *create_input             ( void );

// Constructors
GXInput_t   *load_input               ( const char path[] );
GXInput_t   *load_input_as_json       ( char      *token );
GXBind_t    *load_bind_as_json        ( char      *token );

// Callback registration
int          register_bind_callback   ( GXBind_t *bind       , void                 *function_pointer );

// Callback removal
int          unregister_bind_callback ( GXBind_t *bind       , void                 *function_pointer );

// Keycode finder
SDL_Scancode find_key                 ( const char *name );

// Key information
int          print_all_keys           ( void );
int          print_all_binds          ( GXInput_t    *inputs );

// Input processing
int          process_input            ( GXInstance_t *instance );

// Bind firing
int          fire_bind                ( GXBind_t     *bind    , callback_parameter_t input, GXInstance_t* instance);

// Bind operations
int          append_bind              ( GXInput_t    *input   , GXBind_t            *bind );
GXBind_t    *find_bind                ( GXInput_t    *input   , char                *name );
int          remove_bind              ( GXInput_t    *input   , GXBind_t            *bind );

// Deallocators
int          destroy_bind             ( GXBind_t     *bind );
int          destroy_input            ( GXInput_t    *input ); 