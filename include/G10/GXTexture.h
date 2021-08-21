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
    char        *name;

    // For OpenGL
    unsigned int textureID;
    unsigned int textureUnitIndex;

    // Resolution
    size_t       width;
    size_t       height;

    // Color depth
    size_t       bitsPerPixel;

    u8           cubemap;
};
typedef struct GXTexture_s GXTexture_t;

// Keeps track of what textures are being used by the GPU
struct GXTextureUnit_s
{
    GXTexture_t **activeTextureBlock;
    unsigned int  activeTextureCount;
};
typedef struct GXTextureUnit_s GXTextureUnit_t;


// Constructors
GXTexture_t *createTexture            ( );                    // ✅ Creates an empty texture
GXTexture_t *loadTexture              ( const char path[] );  // ✅ Loads a texture from the disk

// Texture streaming
unsigned int loadTextureToTextureUnit ( GXTexture_t *image ); // ✅ Binds a texture to a texture unit, returns texture unit

// Destructors
int          unloadTexture            ( GXTexture_t *image ); // ✅ Unloads a texture and all of its contents

