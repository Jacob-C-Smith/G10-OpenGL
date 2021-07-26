#pragma once

#include <stdbool.h>

#include <G10/GXLinear.h>


enum colliderType_e
{
    box      = 1,
    sphere   = 2,
    capsule  = 3,
    cylinder = 4,
    cone     = 5
};
typedef enum colliderType_e colliderType_t;

struct GXCollider_s
{
    colliderType_t type;
    GXvec3_t       collisionVector;
};
typedef struct GXCollider_s GXCollider_t;

GXCollider_t* createCollider     ( );                          // ❌ Creates an empty collider

GXCollider_t* loadCollider       ( const char   *path );       // ❌ Creates and loads a collider from a path and populates it
GXCollider_t* loadColliderAsJSON ( char         *token );      // ❌ Creates a collider from JSON text

extern bool   AVXCollision       ( void         *a, void *b ); // ❌ Detects a collision between two objects using AVX

GXCollider_t* removeCollider     ( /*GXEntity_t* entity*/ );   // ❌ Removes a collider from an entity 
int           destroyCollider    ( GXCollider_t *collider );   // ❌ Destroys a collider