#include <G10/GXCamera.h>
#include <G10/GXScene.h>



GXmat4_t perspective ( float fov, float aspect, float near, float far )
{
    /* Compute perspective projection, where f = fov, a = aspect, n = near, and r = far
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

GXCamera_t* createCamera ( )
{
    // Allocate space for a camera struct
    GXCamera_t* ret  = malloc(sizeof(GXCamera_t));
    
    if (ret == 0)
        return ret;

    return ret;
}

GXCamera_t* computeProjectionMatrix ( GXCamera_t* camera )
{
    // Compute and set the projection matrix for the camera
    camera->projection = perspective(camera->fov, camera->aspectRatio, camera->near, camera->far);

    return camera;
}

int destroyCamera ( GXCamera_t* camera )
{
    // View
    camera->where       = (GXvec3_t){ 0.f,0.f,0.f };
    camera->target      = (GXvec3_t){ 0.f,0.f,0.f };
    camera->up          = (GXvec3_t){ 0.f,0.f,0.f };

    // Projection
    camera->fov         = 0.f;
    camera->near        = 0.f;
    camera->far         = 0.f;
    camera->aspectRatio = 0.f;

    // Matricies
    camera->view        = (GXmat4_t) { 0.f,0.f,0.f,0.f,
                                       0.f,0.f,0.f,0.f,
                                       0.f,0.f,0.f,0.f,
                                       0.f,0.f,0.f,0.f };

    camera->projection  = (GXmat4_t) { 0.f,0.f,0.f,0.f,
                                        0.f,0.f,0.f,0.f,
                                        0.f,0.f,0.f,0.f,
                                        0.f,0.f,0.f,0.f };

    // Set next to zero
    camera->next        = (void*) 0;

    return 0;
}
