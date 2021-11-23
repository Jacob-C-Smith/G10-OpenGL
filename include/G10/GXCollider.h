#pragma once

#include <stdbool.h>

#include <G10/G10.h>
#include <G10/GXBVH.h>
#include <G10/GXLinear.h>


enum colliderType_e
{
    box        = 1,
    sphere     = 2,
    capsule    = 3,
    cylinder   = 4,
    cone       = 5,
    convexhull = 6
};
typedef enum colliderType_e colliderType_t;

struct GXCollider_s
{
    colliderType_t type;
    GXBV_t        *bv;
    
};

GXCollider_t* createCollider     ( void );                     // ❌ Creates an empty collider

GXCollider_t* loadCollider       ( const char   *path );       // ❌ Creates and loads a collider from a path and populates it
GXCollider_t* loadColliderAsJSON ( char         *token );      // ❌ Creates a collider from JSON text

int           destroyCollider    ( GXCollider_t *collider );   // ❌ Destroys a collider