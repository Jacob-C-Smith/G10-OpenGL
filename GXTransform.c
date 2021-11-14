#include <G10/GXTransform.h>

GXTransform_t *createTransform       ( vec3           location, quaternion rotation, vec3 scale )
{
    // Allocate space
    GXTransform_t* ret = calloc(1,sizeof(GXTransform_t));

    // Check if valid
    if (ret == 0)
        return (void*)0;

    // Set location, rotation, and scale
    ret->location = location;
    ret->rotation = rotation;
    ret->scale    = scale;

    // Create a model matrix from the location, rotation, and scale
    ret->modelMatrix = mat4xmat4(rotationMatrixFromQuaternion(rotation), translationScaleMat(location, scale));

    // Debug information
    #ifndef NDEBUG
        gPrintLog("[G10] [Transform] Transform created with location (%f,%f,%f)\n" 
                  "                                         rotation (%f,%f,%f,%f)\n"
                  "                                         scale    (%f,%f,%f)\n"
                  , location.x, location.y, location.z, rotation.u, rotation.i, rotation.j, rotation.k, scale.x, scale.y, scale.z);
    #endif

    return ret;
}

GXTransform_t *loadTransform         ( const char     path[] )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(path==(void*)0)
                goto noArgument;
        #endif
    }

    // Uninitialized data
    u8*            data;
    GXTransform_t* ret;

    // Initialized data
    size_t         i = 0;
    FILE*          f = fopen(path, "rb");

    // Load up the file
    i    = gLoadFile(path, 0, false);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data, false);

    ret = loadTransformAsJSON(data);

    // Finish up
    free(data);

    return ret;
    
    // Error handling
    {
        // File not found
        invalidFile:
        #ifndef NDEBUG
            gPrintError("[G10] [Transform] Failed to load file %s\n", path);
        #endif
        return 0;

        // Path was null pointer
        noArgument:
        #ifndef NDEBUG
            gPrintError("[G10] [Transform] No file supplied for path in function %s\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXTransform_t *loadTransformAsJSON   ( char          *token ) 
{
    // Initialiazed data
    GXTransform_t* ret        = 0;
    size_t         len        = strlen(token),
                   tokenCount = GXParseJSON(token, len, 0, 0);
    JSONValue_t*   tokens     = calloc(tokenCount, sizeof(JSONValue_t));
    vec3           location   = { 0,0,0 },
                   scale      = { 0,0,0 };
    quaternion     q          = { 0,0,0,0 };

    // Parse JSON Values
    GXParseJSON(token, len, tokenCount, tokens);

    // Find and assign location, rotation, and scale vectors
    for (size_t k = 0; k < tokenCount; k++)
        if (strcmp("location", tokens[k].name) == 0)
            location = (vec3){ (float)atof(tokens[k].content.aWhere[0]), (float)atof(tokens[k].content.aWhere[1]), (float)atof(tokens[k].content.aWhere[2]) };
        else if (strcmp("rotation", tokens[k].name) == 0)
            q = makeQuaternionFromEulerAngle((vec3) { (float)atof(tokens[k].content.aWhere[0]), (float)atof(tokens[k].content.aWhere[1]), (float)atof(tokens[k].content.aWhere[2]) });
        else if (strcmp("quaternion", tokens[k].name) == 0)
            q = (quaternion){ (float)atof(tokens[k].content.aWhere[0]), (float)atof(tokens[k].content.aWhere[1]), (float)atof(tokens[k].content.aWhere[2]) , (float)atof(tokens[k].content.aWhere[3]) };
        else if ( strcmp( "scale", tokens[k].name ) == 0 )
            scale    = (vec3) { ( float ) atof(tokens[k].content.aWhere[0] ), ( float ) atof(tokens[k].content.aWhere[1] ), ( float ) atof(tokens[k].content.aWhere[2] ) };
    
    // Free subcontents
    free(tokens);


    // TODO: Rewrite createTransform 
    // Process transform
    return createTransform(location, q, scale);
}

int            rotateAboutQuaternion ( GXTransform_t *transform, quaternion axis, float theta)
{
    /*
    * To rotate around a quaternion, we multiply the transform
    * quaternion a special quaternion called 'p' derived by multiplying
    * u by sin( theta/2 ) and multiplying i, j, and k by cos( theta/2 ).
    * Then, we multiply the inverse of p, henceforth ' p' '. This is analogous
    * to complex multiplication.
    */

    float      halfAngle = theta / 2,
               cosHalf   = cosf(halfAngle),
               sinHalf   = sinf(halfAngle),
               nSinHalf  = -sinf(halfAngle);

    quaternion p         = { cosHalf * axis.u, sinHalf * axis.i, sinHalf * axis.j, sinHalf * axis.k },
               pP        = { cosHalf * axis.u, nSinHalf * axis.i, nSinHalf * axis.j, nSinHalf * axis.k },
               pAxis     = normalizeQuaternion(multiplyQuaternion(p, normalizeQuaternion(transform->rotation)));
    
    transform->rotation  = normalizeQuaternion(multiplyQuaternion(pAxis,pP));
    return 0;
}

int            destroyTransform      ( GXTransform_t *transform )
{
    // Zero set everything
    transform->location    = (vec3)       { 0,0,0 };
    transform->rotation    = (quaternion) { 0,0,0,0 };
    transform->scale       = (vec3)       { 0,0,0 };
    transform->modelMatrix = (mat4)       { 0,0,0,0,
                                            0,0,0,0,
                                            0,0,0,0,
                                            0,0,0,0 };
     
    // Free the transform
    free(transform);

    return 0;
}
