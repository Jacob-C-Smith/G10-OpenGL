#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/GXLinear.h>
#include <JSON/JSON.h>
#include <G10/GXQuaternion.h>

// Because the transform struct will often interact with AVX instructions, 
// the struct must be aligned to 0x10 so that *aps instructions can be used
#pragma pack (push)
#pragma pack (0x10)

// Contains information about a transform
struct GXTransform_s
{
    vec3       location,
               scale;
    quaternion rotation;

    mat4       model_matrix;
};
#pragma pack (pop)


// Allocators
GXTransform_t *create_transform            ( void ); // ✅ Creates a transform from location, rotation, and scale

// Constructors
GXTransform_t *load_transform              ( const char     path[] );
GXTransform_t *load_transform_as_json      ( char          *token );
GXTransform_t *transform_from_loc_quat_sca ( vec3           location , quaternion     rotation, vec3  scale );

void           make_model_matrix           ( mat4          *r,         GXTransform_t *transform );

int            rotate_about_quaternion     ( GXTransform_t *transform, quaternion     axis    , float theta );

// Destructors
int            destroy_transform           ( GXTransform_t *transform );                             // ✅ Destroys a transform