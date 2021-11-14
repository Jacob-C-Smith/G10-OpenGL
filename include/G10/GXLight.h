#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <G10/GXtypedef.h>
#include <G10/GXJSON.h>
#include <G10/GXLinear.h>

#define GX_MAX_LIGHTS 8 // Maximum number of lights to be used by the shader at a time. 
                        // Subject to change with hardware capabilities, graphics settings, etc.

struct GXLight_s
{
    const char       *name;     // The name of the light 

    vec4              color;    // The color of the light, w component is unused
    vec4              location; // The location of the light, w component is last distance from camera

    struct GXLight_s *next;     // The next light in the list
};

GXLight_t *createLight     ( void );                // ✅ Creates a blank light 

GXLight_t *loadLight       ( const char  path [] ); // ✅ Loads a light from a JSON file
GXLight_t *loadLightAsJSON ( char       *token   ); // ✅ Loads a light from a JSON object

int        destroyLight    ( GXLight_t*  light   ); // ✅ Destroys a light