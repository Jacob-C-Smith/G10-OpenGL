#include <G10/GXCamera.h>

GXmat4_t perspective ( float fov, float aspect, float nearClip, float farClip)
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
        0,                              0,                 (-(farClip + nearClip) / (farClip - nearClip)), -1,
        0,                              0,                 (2 * farClip * nearClip / (nearClip - farClip)), 0
    };
}

GXCamera_t* createCamera( )
{
    // Allocate space for a camera struct
    GXCamera_t* ret = calloc(1,sizeof(GXCamera_t));

    // Check the memory
    if (ret == 0)
        return ret;

    // Zero set the link
    ret->next = (void*)0;

    return ret;
}

GXCamera_t* loadCamera( const char* path )
{
    // Initialized data
    GXCamera_t* ret      = 0;
    FILE*       f        = fopen(path, "rb");
    size_t      i        = 0;
    char*       data     = 0;

    // Load up the file
    i = gLoadFile(path, 0);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data);

    // Load the camera from data
    ret = loadCameraAsJSON(data);

    // Free the camera
    free(data);

    return ret;
    invalidFile:
    #ifndef NDEBUG
        gPrintLog("[G10] [Camera] Failed to load file %s\n", path);
    #endif
    return 0;
}

GXCamera_t* loadCameraAsJSON( char* token )
{
    // Initialized data
    GXCamera_t*  ret        = createCamera();
    size_t       len        = strlen(token);
    size_t       tokenCount = GXParseJSON(token, len, 0, (void*)0);
    JSONValue_t* tokens     = calloc(tokenCount, sizeof(JSONValue_t));

    // Parse the camera object
    GXParseJSON(token, len, tokenCount, tokens);

    // Copy relevent data for a camera object
    for (size_t l = 0; l < tokenCount; l++)
    {
        // Parse out the camera name
        if (strcmp("name", tokens[l].name) == 0)
        {
            size_t len = strlen(tokens[l].content.nWhere);
            ret->name  = calloc(len+1, sizeof(u8));
            strncpy(ret->name, (const char*)tokens[l].content.nWhere, len);
            ret->name[len] = '\0';
            continue;
        }

        // Parse out where the camera is
        if (strcmp("where", tokens[l].name) == 0)
        {
            ret->where = (GXvec3_t){ (float)atof(tokens[l].content.aWhere[0]), (float)atof(tokens[l].content.aWhere[1]), (float)atof(tokens[l].content.aWhere[2]) };
            continue;
        }

        // Parse out target
        else if (strcmp("target", tokens[l].name) == 0)
        {
            ret->target = (GXvec3_t){ (float)atof(tokens[l].content.aWhere[0]), (float)atof(tokens[l].content.aWhere[1]), (float)atof(tokens[l].content.aWhere[2]) };
            continue;
        }

        // Parse out up
        else if (strcmp("up", tokens[l].name) == 0)
        {
            ret->up = (GXvec3_t){ (float)atof(tokens[l].content.aWhere[0]), (float)atof(tokens[l].content.aWhere[1]), (float)atof(tokens[l].content.aWhere[2]) };
            continue;
        }

        // Parse out FOV
        else if (strcmp("fov", tokens[l].name) == 0)
        {
            ret->fov = (float)atof(tokens[l].content.nWhere);
            continue;
        }

        // Parse out near clipping plane
        else if (strcmp("near", tokens[l].name) == 0)
        {
            ret->nearClip = (float)atof(tokens[l].content.nWhere);
            continue;
        }

        // Parse out far clipping plane
        else if (strcmp("far", tokens[l].name) == 0)
        {
            ret->farClip = (float)atof(tokens[l].content.nWhere);
            continue;
        }

        // Parse out aspect ratio
        else if (strcmp("aspectRatio", tokens[l].name) == 0)
        {
            ret->aspectRatio = (float)atof(tokens[l].content.nWhere);
            continue;
        }
    }

    // If no aspect ratio is supplied, default to 16:9
    if (ret->aspectRatio == 0.f)
        ret->aspectRatio = 1.77777777f; // 16 / 9 = 1.777 

    if (ret->fov >= 89.99f)
        ret->fov = 89.99f;
    if (ret->fov <= 1.f)
        ret->fov = 1.f;

    // Calculate the first view and projection matrices
    ret->view       = lookAt(ret->where, ret->target, ret->up);
    ret->projection = perspective(toRadians(ret->fov), ret->aspectRatio, ret->nearClip, ret->farClip);

    computeProjectionMatrix(ret);

    free(tokens);

    return ret;
}

void computeProjectionMatrix ( GXCamera_t* camera )
{
    // Compute and set the projection matrix for the camera
    camera->projection = perspective(toRadians(camera->fov), camera->aspectRatio, camera->nearClip, camera->farClip);
}

int destroyCamera ( GXCamera_t* camera )
{
    // View
    camera->where       = (GXvec3_t){ 0.f,0.f,0.f,0.f };
    camera->target      = (GXvec3_t){ 0.f,0.f,0.f,0.f };
    camera->up          = (GXvec3_t){ 0.f,0.f,0.f,0.f };

    // Projection
    camera->fov         = 0.f;
    camera->nearClip    = 0.f;
    camera->farClip     = 0.f;
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

    // Set link to zero
    camera->next        = (void*) 0;

    return 0;
}
