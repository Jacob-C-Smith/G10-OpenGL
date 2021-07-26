#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <G10/GXLinear.h>

#include <G10/arch/x86_64/GXAVXmath.h>

struct GXCamera_s {

    //Name
    char              *name;

    // View
    GXvec3_t           where;
    GXvec3_t           target;
    GXvec3_t           up;

    // Projection
    float              fov;
    float              near;
    float              far;
    float              aspectRatio;

    // Matricies
    GXmat4_t           view;
    GXmat4_t           projection;

    // Next
    struct GXCamera_s *next;
};
typedef struct GXCamera_s GXCamera_t;

GXCamera_t*     createCamera            ( );                                                                           // ✅ Creates a camera object to render a scene
GXCamera_t*     loadCamera              ( const char *path );                                                          // ✅ Loads a camera from a JSON file
GXCamera_t*     loadCameraAsJSON        ( char       *token );                                                         // ✅ Loads a camera from a JSON object
GXmat4_t        perspective             ( float       fov,    float aspect,     float near,   float far );             // ✅ Computes perspective projection matrix from FOV, aspect ratio, near and far clipping planes.
extern void     AVXPerspective          ( GXmat4_t   *ret,    float fov,        float aspect, float near, float far ); // ✅ Creates a camera object to render a scene
void            computeProjectionMatrix ( GXCamera_t *camera );                                                        // ✅ Computes an updated projection matrix
extern void     AVXView                 ( GXvec3_t   *eye,    GXvec3_t *target, GXvec3_t *up, GXmat4_t *result );      // ❌ Computes an updated view matrix using AVX2 
inline GXmat4_t lookAt                  ( GXvec3_t    eye,    GXvec3_t target,  GXvec3_t up )                          // ✅ Computes a view matrix from eye, target, and up vectors
{ 
    // Compute forward direction
    GXvec3_t f = normalize((GXvec3_t) {
        eye.x - target.x,
        eye.y - target.y,
        eye.z - target.z
    });

    // Compute left direction as cross product of up and forward
    GXvec3_t l = normalize(crossProductVec3(up, f));

    // Recompute up
    GXvec3_t u = crossProductVec3(f, l);
    
    // Return the view matrix
    return (GXmat4_t) {
        l.x, u.x, f.x, 0,
        l.y, u.y, f.y, 0,
        l.z, u.z, f.z, 0,
        -dotProductVec3(l,eye), -dotProductVec3(u, eye), -dotProductVec3(f, eye), 1
    };
};
inline void     computeViewMatrix       ( GXCamera_t *camera )                              // ✅ Computes an updated view matrix
{
    // Compute a new view matrix
    camera->view = lookAt(camera->where, camera->target, camera->up);
};
int             destroyCamera           ( GXCamera_t *camera );                                                     // ✅ Destroys a camera