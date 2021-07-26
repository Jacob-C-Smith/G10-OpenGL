#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/GXLinear.h>
#include <G10/GXJSON.h>
#include <G10/GXQuaternion.h>

// Because the transform struct will often interact with AVX instructions, 
// the struct must be aligned to 0x10 so that *aps instructions can be used
#pragma pack (push)
#pragma pack (0x10)

// Contains information about a transform
struct GXTransform_s
{
    GXvec3_t location;
    GXvec3_t rotation;
    GXvec3_t scale;

    GXmat4_t modelMatrix;
};

#pragma pack (pop)

typedef struct GXTransform_s GXTransform_t;

// Constructors
GXTransform_t *createTransform     ( GXvec3_t       location, GXvec3_t rotation, GXvec3_t scale ); // ✅ Creates a transform from location, rotation, and scale

// Loaders
GXTransform_t *loadTransform       ( const char     path[] );
GXTransform_t *loadTransformAsJSON ( char          *token );

// Destructors
int            unloadTransform     ( GXTransform_t *transform );                             // ✅ Destroys a transform