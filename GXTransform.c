#include <G10/GXTransform.h>

GXTransform_t* createTransform ( GXvec3_t location, GXvec3_t rotation, GXvec3_t scale )
{
    // Allocate space
    GXTransform_t* ret = malloc(sizeof(GXTransform_t));

    // Check if valid
    if (ret == 0)
        return (void*)0;

    // Set location, rotation, and scale
    ret->location = location;
    ret->rotation = rotation;
    ret->scale    = scale;

    // Create a model matrix from the location, rotation, and scale

    ret->modelMatrix = mat4xmat4(rotationMatrixFromQuaternion(makeQuaternionFromEulerAngle(rotation)), translationScaleMat(location, scale));

    // Debug information
    #ifndef NDEBUG
        printf("Transform created with location (%f,%f,%f)\n" 
               "                       rotation (%f,%f,%f)\n"
               "                       scale    (%f,%f,%f)\n"
               , location.x, location.y, location.z, rotation.x, rotation.y, rotation.z, scale.x, scale.y, scale.z);
    #endif

    return ret;
}

GXTransform_t* loadTransform( const char path[] )
{


    return (void*)0;
}

GXTransform_t* loadTransformFromJSON(char* token)
{

    return (void*)0;
}



int unloadTransform ( GXTransform_t* transform )
{
    // Zero set everything
    transform->location    = (GXvec3_t){ 0,0,0 };
    transform->rotation    = (GXvec3_t){ 0,0,0 };
    transform->scale       = (GXvec3_t){ 0,0,0 };
    transform->modelMatrix = (GXmat4_t){ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    // Free the transform
    free(transform);

    return 0;
}
