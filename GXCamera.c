#include <G10/GXCamera.h>

GXmat4_t perspective( float fov, float aspect, float near, float far )
{
    /* 
     * Compute perspective projection, where f = fov, a = aspect, n = near, and r = far
     * ┌                                                      ┐
     * │ (a*tan(f/2))^-1, 0,             0,                0, │
     * │ 0,               (tan(f/2))^-1, 0,                0  │
     * │ 0,               0,             -((r+n)/(r-n)),  -1  │
     * │ 0,               0,             -((2*r*n)/(r-n)), 0  │
     * └                                                      ┘
     */

    return (GXmat4_t) {                                                                       
        (1 / (aspect * tanf(fov / 2))), 0,                 0,                               0,
        0,                              1 / tanf(fov / 2), 0,                               0,                 
        0,                              0,                 (-(far + near) / (far - near)), -1,
        0,                              0,                 (2 * far * near / (near - far)), 0
    };
}

GXcamera_t* createCamera( GXvec3_t where, GXvec3_t target, GXvec3_t up, float fov, float near, float far, float aspectRatio )
{
    // Allocate space for a camera struct
    GXcamera_t* ret  = malloc(sizeof(GXcamera_t));
    
    if (ret == 0)
        return (void*)0;

    // Populate struct
    ret->where       = where;
    ret->target      = target;
    ret->up          = up;

    ret->fov         = fov;
    ret->near        = near;
    ret->far         = far; 
    ret->aspectRatio = aspectRatio;

    // Calculate perspective projection
    ret->projection = perspective(fov, aspectRatio, near, far);
    ret->view       = lookAt(where, target, up);
    return ret;
}

GXcamera_t* computeProjectionMatrix( GXcamera_t* camera )
{
    // Compute and set the projection matrix for the camera
    camera->projection = perspective(camera->fov, camera->aspectRatio, camera->near, camera->far);

    return camera;
}