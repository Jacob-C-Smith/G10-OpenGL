#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>

// Contains information about a texture
struct GXTexture_s
{
    // Name
    char                *name;

    // For OpenGL
    unsigned int         texture_id,
                         texture_unit_index;

    // Resolution
    size_t               width,
                         height;

    // Color depth
    size_t               bits_per_pixel;

    u8                   cubemap;
    struct GXTexture_s  *next;
};

// Keeps track of what textures are being used by the GPU
struct GXTextureUnit_s
{
    GXTexture_t **active_texture_block;
    unsigned int  active_texture_count;
};
typedef struct GXTextureUnit_s GXTextureUnit_t;


// Allocators
GXTexture_t *create_texture                   ( void );               // ✅ Creates an empty texture

// Constructors

// NOTE: These constructors are different than the rest of the engine. 
//       load_texture loads a PNG, JPG, Bitmap, or HDR file.
//       load_texture_as_json loads a texture from a token, with
//       special attributes (repeat uv, bits per pixel, etc)
GXTexture_t *load_texture                     ( const char  path[] );  // ✅ Loads a texture from the disk
GXTexture_t *load_texture_as_json             ( char       *token );   // ❌ Constructs a texture from a JSON token


// Texture operations
unsigned int bind_texture_to_unit             ( GXTexture_t *image ); // ✅ Binds a texture to a texture unit, returns texture unit
unsigned int remove_texture_from_texture_unit ( unsigned int index ); // ✅ Unbinds a texture from a texture unit at the specified index

// Destructors
int          unload_texture                   ( GXTexture_t *image ); // ✅ Unloads a texture and all of its contents

