#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL_gamecontroller.h>

#include <G10/G10.h>
#include <JSON/JSON.h>

#include <G10/GXLinear.h>
#include <G10/arch/x86_64/GXAVXmath.h>

struct GXCamera_s {

    // Name
    char              *name;

    // View
    vec3               where,
                       target,
                       up;

    // Projection
    float              fov,
                       near_clip,
                       far_clip,
                       aspect_ratio;

    // Matricies
    mat4               view,
                       projection;

    // Next
    struct GXCamera_s *next;
};

// Allocators
GXCamera_t*     create_camera                     ( void );                                                                                       // ✅ Creates a camera object to render a scene

// Constructors
GXCamera_t*     load_camera                       ( const char *path );                                                                           // ✅ Loads a camera from a JSON file
GXCamera_t*     load_camera_as_json               ( char       *token );                                                                          // ✅ Loads a camera from a JSON object

// Matrix operations
mat4            perspective                       ( float       fov,    float     aspect       , float nearClip , float farClip);                  // ✅ Computes perspective projection matrix from FOV, aspect ratio, near and far clipping planes.
extern void     AVXPerspective                    ( mat4       *ret,    float     fov          , float aspect   , float nearClip, float farClip ); // ✅ Creates a camera object to render a scene
void            computeProjectionMatrix           ( GXCamera_t *camera );                                                                         // ✅ Computes an updated projection matrix

inline mat4     look_at                           ( vec3        eye,    vec3      target       , vec3  up )                                        // ✅ Computes a view matrix from eye, target, and up vectors
{ 
    // Compute forward direction
    vec3 f = normalize((vec3) {
        eye.x - target.x,
        eye.y - target.y,
        eye.z - target.z
    });

    // Compute left direction as cross product of up and forward
    vec3 l = normalize(cross_product_vec3(up, f));

    // Recompute up
    vec3 u = cross_product_vec3(f, l);
    
    // Return the view matrix
    return (mat4) {
        l.x, u.x, f.x, 0,
        l.y, u.y, f.y, 0,
        l.z, u.z, f.z, 0,
        -dot_product_vec3(l,eye), -dot_product_vec3(u, eye), -dot_product_vec3(f, eye), 1
    };
};
inline mat4     q_look_at                         ( vec3        eye,    vec3      target );
extern void     avx_look_at                       ( vec3       *eye,    vec3     *target       , vec3 *up,        mat4 *result );                  // ❌ Computes an updated view matrix using AVX2 
inline void     compute_view_matrix               ( GXCamera_t *camera )                                                                   // ✅ Computes an updated view matrix
{
    // Compute a new view matrix
    camera->view = look_at(camera->where, camera->target, camera->up);
};

// Destructors
int             destroy_camera                    ( GXCamera_t *camera );                                                                         // ✅ Destroys a camera