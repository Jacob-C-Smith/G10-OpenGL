#include <G10/GXCamera.h>

mat4        perspective                    ( float        fov, float       aspect,        float nearClip,  float farClip)
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

GXCamera_t *create_camera                  ( void )
{
    // Allocate space for a camera struct
    GXCamera_t* ret = calloc(1,sizeof(GXCamera_t));

    // Check the memory
    {
        #ifndef NDEBUG
            if (ret == 0)
                goto noMem;
        #endif
    }

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
    // Argument check
    {
        #ifndef NDEBUG
            if(path == (void *)0)
                goto no_path;
        #endif
    }

    // Initialized data
    GXCamera_t *ret      = 0;
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

    // Error handling
    {

        // Argument errors
        {
            no_path:
            #ifndef NDEBUG
                g_print_error("[G10] [Camera] Null pointer provided for \"path\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}

GXCamera_t *load_camera_as_json            ( char        *token )
{

    // Argument check
    {
        #ifndef NDEBUG
            if(token==(void*)0)
                goto no_token;
        #endif
    }

    // Initialized data
    GXCamera_t   *ret          = create_camera();
    size_t        len          = strlen(token),
                  token_count  = parse_json(token, len, 0, (void*)0),
                  i            = 0;
    JSONToken_t  *tokens       = calloc(token_count, sizeof(JSONToken_t));

    char         *name         = 0,
                **location     = 0,
                **target       = 0,
                **up           = 0,
                 *fov          = 0,
                 *near         = 0,
                 *far          = 0,
                 *aspect_ratio = 0;
                

    // Parse the camera object
    parse_json(token, len, token_count, tokens);

    // Copy relevent data for a camera object
    for (i = 0; i < token_count; i++)
    {

        // Parse out the camera name
        if      ( strcmp("name"        , tokens[i].key) == 0 )
        {
            if (tokens[i].type == JSONstring)
                name = tokens[i].value.n_where;
            else
                goto name_type_error;

            continue;
        }

        // Parse out where the camera is
        else if ( strcmp("where"       , tokens[i].key) == 0 )
        {
            if (tokens[i].type == JSONarray)
                location = tokens[i].value.a_where;
            else
                goto location_type_error;

            continue;
        }

        // Parse out target
        else if ( strcmp("target"      , tokens[i].key) == 0 )
        {
            if (tokens[i].type == JSONarray)
                target = tokens[i].value.a_where;
            else
                goto target_type_error;

            continue;
        }

        // Parse out up
        else if ( strcmp("up"          , tokens[i].key) == 0 )
        {
            if (tokens[i].type == JSONarray)
                up = tokens[i].value.a_where;
            else
                goto up_type_error;

            continue;
        }

        // Parse out FOV
        else if ( strcmp("fov"         , tokens[i].key) == 0 )
        {
            if (tokens[i].type == JSONprimative)
                fov = tokens[i].value.n_where;
            else
                goto fov_type_error;

            continue;
        }

        // Parse out near clipping plane
        else if ( strcmp("near"        , tokens[i].key) == 0 )
        {
            if (tokens[i].type == JSONprimative)
                near = tokens[i].value.n_where;
            else
                goto near_type_error;

            continue;
        }

        // Parse out far clipping plane
        else if ( strcmp("far"         , tokens[i].key) == 0 )
        {
            if (tokens[i].type == JSONprimative)
                far = tokens[i].value.n_where;
            else
                goto far_type_error;

            continue;
        }

        // Parse out aspect ratio
        else if ( strcmp("aspect ratio", tokens[i].key) == 0 )
        {
            if (tokens[i].type == JSONprimative)
                aspect_ratio = tokens[i].value.n_where;
            else
                goto aspect_ratio_type_error;

            continue;
        }

        loop:
    }

    // Construct the camera
    {

        // Set the name
        {

            if (name)
            {
                size_t name_len = strlen(name);
                ret->name       = calloc(name_len + 1, sizeof(u8));

                // Check allocated memory
                {
                    #ifndef NDEBUG
                        if (ret->name == (void*)0)
                            goto no_mem;
                    #endif
                }

                strncpy(ret->name, name, name_len);
            }
            else
                goto no_name;
        }

        // Set the camera location
        {

            if (location)
                ret->location = (vec3){ (float)atof(location[0]), (float)atof(location[1]), (float)atof(location[2]) };
            else
                goto no_location;
        }

        // Set the camera target
        {

            if (target)
                ret->target = (vec3){ (float)atof(target[0]), (float)atof(target[1]), (float)atof(target[2]) };

            else
                goto no_target;
        }

        // Set the up directon
        {

            if (up)
                ret->up = (vec3){ (float)atof(up[0]), (float)atof(up[1]), (float)atof(up[2]) };

            // Assume up is < 0, 0, 1 > if no up vector is supplied
            else
                ret->up = (vec3){ 0.f, 0.f, 1.f };
        }

        // Set the camera fov
        {

            if (fov)
            {

                ret->fov = (float)atof(fov);

                // Bounds check for the fov
                {
                    if (ret->fov >= 89.99f)
                        ret->fov = 89.99f;
                    if (ret->fov <= 1.f)
                        ret->fov = 1.f;
                }

            }

            // Assume 30 degree fov if no fov is supplied
            else
                ret->fov = 30.f;
        }

        // Set the near clipping plane
        {

            if (near)
                ret->near_clip = (float)atof(near);

            // Assume 0.1 unit clip if no near clip is supplied
            else
                ret->near_clip = 0.1f;
        }
        
        // Set the far clipping plane
        {

            if (far)
                ret->far_clip = (float)atof(far);

            // Assume 1000.0 unit clip if no far clip is supplied
            else
                ret->far_clip = 1000.f;
        }

        // Set the aspect ratio
        {

            if (aspect_ratio)
                ret->aspect_ratio = (float)atof(aspect_ratio);

            // Assume 16:9 aspect ratio
            else
                ret->aspect_ratio = 1.777777777f;
        }

        // Calculate the first view and projection matrices
        {
            ret->view = look_at(ret->location, ret->target, ret->up);
            ret->projection = perspective(to_radians(ret->fov), ret->aspect_ratio, ret->near_clip, ret->far_clip);
        }
    }

    free(tokens);

    return ret;

    // Error handling
    {

        // Standard library errors
        {
            no_mem:
            #ifndef NDEBUG
                g_print_error("[Standard Library] Out of memory in call to function \"%s\"\n", __FUNCSIG__);
                return 0;
            #endif
        }

        // Argument errors
        {
            no_token:
            #ifndef NDEBUG
                g_print_error("[G10] [Camera] Null pointer provided for \"token\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}

int         look_at_entity                 ( GXCamera_t *camera, GXEntity_t *entity)
{
    camera->view = look_at(camera->location, entity->transform->location, (vec3) { 0.f, 0.f, 1.f });
    
    return 0;
}

int         destroy_camera                 ( GXCamera_t *camera )
{
    
    // Argument check
    {
        if(camera == (void*)0)
            goto no_camera;
    }

    free(camera->name);
    
    free(camera);

    return 0;

    // Argument errors
    {
        no_camera:
        #ifndef NDEBUG
            g_print_error("[G10] [Camera] Null pointer provided for \"camera\" in call to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}
