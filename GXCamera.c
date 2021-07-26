#include <G10/GXCamera.h>


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

GXCamera_t* createCamera( )
{
    // Allocate space for a camera struct
    GXCamera_t* ret = malloc(sizeof(GXCamera_t));

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

    // Load the file
    {
        // Check if file is valid
        if (f == NULL)
            goto invalidFile;

        // Find file size and prep for read
        fseek(f, 0, SEEK_END);
        i = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Allocate data and read file into memory
        data = malloc(i);
        if (data == 0)
            return (void*)0;
        fread(data, 1, i, f);
    
        // We no longer need the file
        fclose(f);
    }

    // Load the camera from data
    ret = loadCameraAsJSON(data);

    // Free the camera
    free(data);

    return ret;
    invalidFile:
    #ifndef NDEBUG
        printf("[G10] [Camera] Failed to load file %s\n", path);
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
            ret->name  = malloc(len+1);
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
            ret->near = (float)atof(tokens[l].content.nWhere);
            continue;
        }

        // Parse out far clipping plane
        else if (strcmp("far", tokens[l].name) == 0)
        {
            ret->far = (float)atof(tokens[l].content.nWhere);
            continue;
        }

        // Parse out aspect ratio
        else if (strcmp("aspectRatio", tokens[l].name) == 0)
        {
            ret->aspectRatio = (float)atof(tokens[l].content.nWhere);
            continue;
        }
    }

    // Calculate the first view and projection matrices
    ret->view       = lookAt(ret->where, ret->target, ret->up);
    ret->projection = perspective(ret->fov, ret->aspectRatio, ret->near, ret->far);

    // If no aspect ratio is supplied, default to 16:9
    if (ret->aspectRatio == 0.f)
        ret->aspectRatio = 1.77777777f; // 16 / 9 = 1.777 
    
    free(tokens);

    return ret;
}

void computeProjectionMatrix ( GXCamera_t* camera )
{
    // Compute and set the projection matrix for the camera
    camera->projection = perspective(toRadians(camera->fov), camera->aspectRatio, camera->near, camera->far);
}

int destroyCamera ( GXCamera_t* camera )
{
    // View
    camera->where       = (GXvec3_t){ 0.f,0.f,0.f,0.f };
    camera->target      = (GXvec3_t){ 0.f,0.f,0.f,0.f };
    camera->up          = (GXvec3_t){ 0.f,0.f,0.f,0.f };

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

    // Set link to zero
    camera->next        = (void*) 0;

    return 0;
}
