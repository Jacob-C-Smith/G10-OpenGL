#include <G10/GXCamera.h>

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
    // TODO: Error handling

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

            strncpy(ret->name, (const char*)tokens[l].value.n_where, len);
            ret->name[len] = '\0';
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

int         update_camera_from_keyboard_input ( GXCamera_t *camera, const u8 *keyboardState, float deltaTime )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(camera == (void*)0)
                goto noCamera;
            if (keyboardState == (void*)0)
                goto noKeyboardState;
        #endif  
    }

    // Uninitialized data
    vec2  orientation = { 0.f, 0.f };
    vec3 *velocity,
         *acceleration;

    // Initialized data

    // Turn keyboard input into orientation 
    {
        if (keyboardState[SDL_SCANCODE_W])
            if (keyboardState[SDL_SCANCODE_A])
                orientation = (vec2){ (float)-M_SQRT1_2, (float)M_SQRT1_2 }; // ↖ WA
            else if (keyboardState[SDL_SCANCODE_D])
                orientation = (vec2){ (float)M_SQRT1_2, (float)M_SQRT1_2 };  // ↗ WD
            else
                orientation = (vec2){ 0.f, 1.f };                            // ↑ W
        else if (keyboardState[SDL_SCANCODE_A])
            if (keyboardState[SDL_SCANCODE_S])
                orientation = (vec2){ (float)-M_SQRT1_2,(float)-M_SQRT1_2 }; // ↙ AS
            else
                orientation = (vec2){ -1.f, 0.f };                           // ← A
        else if (keyboardState[SDL_SCANCODE_D])
            if (keyboardState[SDL_SCANCODE_S])
                orientation = (vec2){ (float)M_SQRT1_2, (float)-M_SQRT1_2 }; // ↘ DS
            else
                orientation = (vec2){ 1.f, 0.f };                            // → D
        else if (keyboardState[SDL_SCANCODE_S])
            orientation = (vec2){ 0.f, -1.f };                               // ↓ S
        else
            return 0;
    }

    // Uncomment for orientation coords with newline
    // printf("( %.2f, %.2f )\n", orientation.x, orientation.y);
    
    // Uncomment for orientation coordinates without newline. 
    // printf("( %.2f, %.2f )\r", orientation.x, orientation.y);

    // Turn orientation into acceleration
    camera->acceleration.x = 1.f * orientation.x * camera->view.a + -orientation.y * 1.f * camera->view.c,
    camera->acceleration.y = 1.f * orientation.x * camera->view.e + -orientation.y * 1.f * camera->view.g;
    
    // Air resistance
    if (camera->velocity.x || camera->velocity.y)
        if (camera->velocity.x < 0.f)
            camera->acceleration.x += 0.001f;
        else
            camera->acceleration.x -= 0.001f;
        if (camera->velocity.y < 0.f)
            camera->acceleration.y += 0.001f;
        else
            camera->acceleration.y -= 0.001f;


    // Calculate v proj a                                                                                                                                                                                                                                                                                                                                      
    float n = AVXDot(&camera->acceleration, &camera->velocity);
    float d = length(camera->acceleration);
    d *= d;
    float c = length(camera->velocity);
    
    float cosTheta = n / (c * d);

    float lenvPa   = length(mul_vec3_f(camera->acceleration, n / d));
    float lenAdT   = length(mul_vec3_f(camera->acceleration, deltaTime));
    float speedLim = 60000;

    if (lenvPa < speedLim - lenAdT)
    {
        camera->velocity = mul_vec3_f(camera->acceleration, deltaTime);
    }
    else if (speedLim - lenAdT <= lenvPa < speedLim)
    {
        camera->velocity = mul_vec3_f(mul_vec3_f(camera->acceleration, (speedLim - c * cosTheta) / d), deltaTime);
    }

    camera->where.x += camera->velocity.x * deltaTime,
    camera->where.y += camera->velocity.y * deltaTime;

    vec3 tw;
    add_vec3(&tw, camera->target, camera->where);

    camera->view = look_at(camera->where, tw, camera->up);

    return 0;

    // Error handling
    {
        noCamera:
        #ifndef NDEBUG
            g_print_error("[G10] [Camera] Parameter \"camera\" is null in call to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
        
        noKeyboardState:
        #ifndef NDEBUG
            g_print_error("[G10] [Camera] Parameter \"keyboardState\" is null in call to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int         destroy_camera                 ( GXCamera_t* camera )
{
    // Name
    free(camera->name);
    camera->name         = (void*)0;

    // Displacement derivatives
    camera->acceleration = (vec3){ 0.f,0.f,0.f,0.f },
    camera->velocity     = (vec3){ 0.f,0.f,0.f,0.f };

    // View
    camera->where       = (vec3){ 0.f,0.f,0.f,0.f };
    camera->target      = (vec3){ 0.f,0.f,0.f,0.f };
    camera->up          = (vec3){ 0.f,0.f,0.f,0.f };

    // Projection
    camera->fov         = 0.f;
    camera->near_clip    = 0.f;
    camera->far_clip     = 0.f;
    camera->aspect_ratio = 0.f;

    // Matricies
    camera->view        = (mat4) { 0.f,0.f,0.f,0.f,
                                   0.f,0.f,0.f,0.f,
                                   0.f,0.f,0.f,0.f,
                                   0.f,0.f,0.f,0.f };

    camera->projection  = (mat4) { 0.f,0.f,0.f,0.f,
                                   0.f,0.f,0.f,0.f,
                                   0.f,0.f,0.f,0.f,
                                   0.f,0.f,0.f,0.f };

    // Set link to zero
    camera->next        = (void*) 0;

    free(camera);

    return 0;
}
