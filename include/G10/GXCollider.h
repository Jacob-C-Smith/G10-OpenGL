#pragma once

#include <stdbool.h>

#include <G10/G10.h>
#include <G10/GXBVH.h>
#include <G10/GXLinear.h>


enum colliderType_e
{
    plane      = 1,
    box        = 2,
    sphere     = 3,
    capsule    = 4,
    cylinder   = 5,
    cone       = 6,
    convexhull = 7,
    invalid    = 0
};
typedef enum colliderType_e colliderType_t;

struct GXCollider_s
{
    GXBV_t         *bv;
    colliderType_t  type;
    void           *convex_hull;
    vec3            dimensions;
};

GXCollider_t* create_collider       ( void );                     // ❌ Creates an empty collider

GXCollider_t* load_collider         ( const char   *path );       // ❌ Creates and loads a collider from a path and populates it
GXCollider_t* load_collider_as_json ( char         *token );      // ❌ Creates a collider from JSON text

int           destroy_collider      ( GXCollider_t *collider );   // ❌ Destroys a collider