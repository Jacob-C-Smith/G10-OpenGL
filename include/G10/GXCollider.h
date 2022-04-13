#pragma once

#include <stdbool.h>

#include <G10/G10.h>
#include <G10/GXBVH.h>
#include <G10/GXCollision.h>
#include <G10/GXLinear.h>


enum colliderType_e
{
    quad       = 1,
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
    // 
    colliderType_t  type;

    // Dimensions
    vec3            dimensions;

    // AABB dimensions
    vec3            aabb_dimensions;

    mat4           *model;

    // List of points on convex hull
    vec3           *convex_hull;
    size_t          convex_hull_count;

    // AABB data
    size_t          aabb_start_callback_count,
                    aabb_callback_count,
                    aabb_end_callback_count,

                    aabb_start_callback_max,
                    aabb_callback_max,
                    aabb_end_callback_max;

    void          **aabb_start_callbacks,
                  **aabb_callbacks,
                  **aabb_end_callbacks;

    // Pointer to bounding volume in the scene BVH
    GXBV_t         *bv;
};

// Allocators
GXCollider_t *create_collider              ( void );                     // ❌ Creates an empty collider

// Constructors
GXCollider_t *load_collider                ( const char   *path );       // ❌ Creates and loads a collider from a path and populates it
GXCollider_t *load_collider_as_json        ( char         *token );      // ❌ Creates a collider from JSON text

// Callbacks
int           add_start_collision_callback ( GXCollider_t *collider, void* function_pointer );
int           add_collision_callback       ( GXCollider_t *collider, void* function_pointer );
int           add_end_collision_callback   ( GXCollider_t *collider, void* function_pointer );

// Destructors
int           destroy_collider             ( GXCollider_t *collider );   // ❌ Destroys a collider