#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <G10/G10.h>
#include <G10/GXJSON.h>

#include <G10/GXLinear.h>
#include <G10/arch/x86_64/GXAVXmath.h>

struct GXCamera_s {

    // Name
    char              *name;

    // Displacement derivatives
    vec3               acceleration,
                       velocity;

    // View
    vec3               where,
                       target,
                       up;

    // Projection
    float              fov,
                       nearClip,
                       farClip,
                       aspectRatio;

    // Matricies
    mat4               view,
                       projection;

    // Next
    struct GXCamera_s *next;
};

GXCamera_t*     createCamera            ( );                                                                                     // ✅ Creates a camera object to render a scene

GXCamera_t*     loadCamera              ( const char *path );                                                                    // ✅ Loads a camera from a JSON file
GXCamera_t*     loadCameraAsJSON        ( char       *token );                                                                   // ✅ Loads a camera from a JSON object

mat4            perspective             ( float       fov,    float aspect,     float nearClip, float farClip);                  // ✅ Computes perspective projection matrix from FOV, aspect ratio, near and far clipping planes.
extern void     AVXPerspective          ( mat4       *ret,    float fov,        float aspect,   float nearClip, float farClip ); // ✅ Creates a camera object to render a scene
void            computeProjectionMatrix ( GXCamera_t *camera );                                                                  // ✅ Computes an updated projection matrix

inline mat4     lookAt                  ( vec3        eye,    vec3  target,     vec3 up )                                           // ✅ Computes a view matrix from eye, target, and up vectors
{ 
    // Compute forward direction
    vec3 f = normalize((vec3) {
        eye.x - target.x,
        eye.y - target.y,
        eye.z - target.z
    });

    // Compute left direction as cross product of up and forward
    vec3 l = normalize(crossProductVec3(up, f));

    // Recompute up
    vec3 u = crossProductVec3(f, l);
    
    // Return the view matrix
    return (mat4) {
        l.x, u.x, f.x, 0,
        l.y, u.y, f.y, 0,
        l.z, u.z, f.z, 0,
        -dotProductVec3(l,eye), -dotProductVec3(u, eye), -dotProductVec3(f, eye), 1
    };
};
inline mat4     qLookAt                 ( vec3        eye,    vec3  target );
extern void     AVXLookAt               ( vec3       *eye,    vec3 *target, vec3 *up, mat4 *result );                // ❌ Computes an updated view matrix using AVX2 
inline void     computeViewMatrix       ( GXCamera_t *camera )                              // ✅ Computes an updated view matrix
{
    // Compute a new view matrix
    camera->view = lookAt(camera->where, camera->target, camera->up);
};

int             updateCameraFromInput   ( GXCamera_t *camera, const u8 *keyboardState, float deltaTime );

int             destroyCamera           ( GXCamera_t *camera );                                                                  // ✅ Destroys a camera