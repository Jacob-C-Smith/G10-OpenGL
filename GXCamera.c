﻿#include <G10/GXCamera.h>

mat4        perspective                   ( float        fov, float       aspect,        float nearClip,  float farClip)
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

    return (mat4) {                                                                       
        (1 / (aspect * tanf(fov / 2))), 0,                 0,                               0,
        0,                              1 / tanf(fov / 2), 0,                               0,                 
        0,                              0,                 (-(farClip + nearClip) / (farClip - nearClip)), -1,
        0,                              0,                 (2 * farClip * nearClip / (nearClip - farClip)), 0
    };
}

GXCamera_t *create_camera                  ( )
{
    // Allocate space for a camera struct
    GXCamera_t* ret = calloc(1,sizeof(GXCamera_t));

    // Check the memory
    #ifndef NDEBUG
        if (ret == 0)
            goto noMem;
    #endif

    return ret;

    // Error handling
    {
        noMem:
        #ifndef NDEBUG
            g_print_error("[G10] [Camera] Out of memory.\n");
        #endif
        return 0;
    }
}

GXCamera_t *load_camera                    ( const char  *path )
{
    // Initialized data
    GXCamera_t *ret      = 0;
    FILE       *f        = fopen(path, "rb");
    size_t      i        = 0;
    char       *data     = 0;

    // Load up the file
    i    = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    // Load the camera from data
    ret = load_camera_as_json(data);

    // Free the camera
    free(data);

    return ret;    
}

GXCamera_t *load_camera_as_json              ( char        *token )
{
    // Initialized data
    GXCamera_t*  ret        = create_camera();
    size_t       len        = strlen(token);
    size_t       tokenCount = parse_json(token, len, 0, (void*)0);
    JSONToken_t* tokens     = calloc(tokenCount, sizeof(JSONToken_t));

    // Parse the camera object
    parse_json(token, len, tokenCount, tokens);

    // Copy relevent data for a camera object
    for (size_t l = 0; l < tokenCount; l++)
    {
        // Parse out the camera name
        if (strcmp("name", tokens[l].key) == 0)
        {
            size_t len = strlen(tokens[l].value.n_where);
            ret->name  = calloc(len+1, sizeof(u8));

            // Check allocated memory
            {
                #ifndef NDEBUG
                    if(ret->name == (void*)0)
                        goto noMem;
                #endif
            }

            strncpy(ret->name, tokens[l].value.n_where, len);
            continue;
        }

        // Parse out where the camera is
        if (strcmp("where", tokens[l].key) == 0)
        {
            ret->where = (vec3){ (float)atof(tokens[l].value.a_where[0]), (float)atof(tokens[l].value.a_where[1]), (float)atof(tokens[l].value.a_where[2]) };
            continue;
        }

        // Parse out target
        else if (strcmp("target", tokens[l].key) == 0)
        {
            ret->target = (vec3){ (float)atof(tokens[l].value.a_where[0]), (float)atof(tokens[l].value.a_where[1]), (float)atof(tokens[l].value.a_where[2]) };
            continue;
        }

        // Parse out up
        else if (strcmp("up", tokens[l].key) == 0)
        {
            ret->up = (vec3){ (float)atof(tokens[l].value.a_where[0]), (float)atof(tokens[l].value.a_where[1]), (float)atof(tokens[l].value.a_where[2]) };
            continue;
        }

        // Parse out FOV
        else if (strcmp("fov", tokens[l].key) == 0)
        {
            ret->fov = (float)atof(tokens[l].value.n_where);
            continue;
        }

        // Parse out near clipping plane
        else if (strcmp("near", tokens[l].key) == 0)
        {
            ret->near_clip = (float)atof(tokens[l].value.n_where);
            continue;
        }

        // Parse out far clipping plane
        else if (strcmp("far", tokens[l].key) == 0)
        {
            ret->far_clip = (float)atof(tokens[l].value.n_where);
            continue;
        }

        // Parse out aspect ratio
        else if (strcmp("aspect ratio", tokens[l].key) == 0)
        {
            ret->aspect_ratio = (float)atof(tokens[l].value.n_where);
            continue;
        }
    }

    // If no aspect ratio is supplied, default to 16:9
    if (ret->aspect_ratio == 0.f)
        ret->aspect_ratio = 1.77777777f; // 16 / 9 = 1.777 

    // Make sure that fov is inside of the maximium or minimum range.
    if (ret->fov >= 89.99f)
        ret->fov = 89.99f;
    if (ret->fov <= 1.f)
        ret->fov = 1.f;

    // Calculate the first view and projection matrices
    ret->view       = look_at(ret->where, ret->target, ret->up);
    ret->projection = perspective(to_radians(ret->fov), ret->aspect_ratio, ret->near_clip, ret->far_clip);

    computeProjectionMatrix(ret);

    free(tokens);

    return ret;

    // Error handling
    {
        #ifndef NDEBUG
            noMem:
            g_print_error("[G10] [Camera] Out of memory in call to function \"%s\"\n",__FUNCSIG__);
            return 0;
        #endif
    }
}

void        computeProjectionMatrix       ( GXCamera_t* camera )
{
    // Compute and set the projection matrix for the camera
    camera->projection = perspective(to_radians(camera->fov), camera->aspect_ratio, camera->near_clip, camera->far_clip);
}



int         destroy_camera                 ( GXCamera_t* camera )
{
    free(camera->name);

    free(camera);

    return 0;
}
