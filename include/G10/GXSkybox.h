#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/GXCamera.h>
#include <G10/GXTexture.h>
#include <G10/GXLinear.h>
#include <G10/GXShader.h>
#include <G10/GXHDR.h>

#define SKYBOX_RESOLUTION

struct GXSkybox_s
{
    char        *name;

    GXPart_t    *cube,
                *quad;
   
    GXShader_t  *environment,
                *irradiance,
                *prefilter,
                *background,
                *brdf;

    GXTexture_t *environment_cubemap,
                *irradiance_cubemap,
                *prefilter_cubemap,
                *lut;

    mat4         projection;
    mat4         views[6];
    vec3         color;
};

// Allocators
GXSkybox_t *create_skybox       ( void );                                    // ❌ Creates an empty skybox

// Constructors
GXSkybox_t *load_skybox         ( const char  path[] );                      // ❌ Loads a skybox from a file
GXSkybox_t *load_skybox_as_json ( char       *token );                       // ✅ Loads a skybox from a JSON token

// Graphical operations
int         draw_skybox         ( GXSkybox_t *skybox );  // ✅ Draws the skybox 

// Destructors
int         destroy_skybox      ( GXSkybox_t* skybox );                      // ✅ Destroys the skybox