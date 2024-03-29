#include <G10/GXRigidbody.h>

GXRigidbody_t *create_rigidbody           ( )
{
    // Initialized data
    GXRigidbody_t* ret = calloc(1,sizeof(GXRigidbody_t));

    if (ret == (void*)0)
        return ret;

    ret->mass                = 0.f;
    ret->radius              = 0.f;

    ret->acceleration        = (vec3){ 0.f, 0.f, 0.f, 0.f };
    ret->velocity            = (vec3){ 0.f, 0.f, 0.f, 0.f };
    
    ret->angular_acceleration = (quaternion){ 0.f, 0.f, 0.f, 0.f };
    ret->angular_velocity     = (quaternion){ 0.f, 0.f, 0.f, 0.f };

    ret->forces              = calloc(MAXFORCES, sizeof(vec3));
    ret->forces_count        =  1;

    ret->torques             = calloc(MAXFORCES, sizeof(vec3));

    return ret;
}

GXRigidbody_t *load_rigidbody             ( const char     path[] )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(path == (void*)0)
                goto no_path;
        #endif
    }
    
    // Initialized data
    GXRigidbody_t* ret      = 0;
    FILE*          f        = fopen(path, "rb");
    size_t         i        = 0;
    char*          data     = 0;

    // Load up the file
    i    = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    // Load the camera from data
    ret = load_rigidbody_as_json(data);

    // Free the camera
    free(data);

    return ret;

    invalidFile:
    #ifndef NDEBUG
        g_print_error("[G10] [Rigidbody] Failed to load file %s\n", path);
    #endif
    return 0;

    // Error handling
    {

        // Argument error
        {
            no_path:
            #ifndef NDEBUG
                g_print_error("[G10] [Rigidbody] Null pointer provided for \"path\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        }
    }
}

GXRigidbody_t *load_rigidbody_as_json       ( char          *token )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(token==(void*)0)
                goto no_token;
        #endif
    }
    
    // Initialized data
    GXRigidbody_t*  ret        = create_rigidbody();
    size_t          len        = strlen(token);
    size_t          token_count = parse_json(token, len, 0, (void*)0);
    JSONToken_t*    tokens     = calloc(token_count, sizeof(JSONToken_t));

    // Parse the rigid body  object
    parse_json(token, len, token_count, tokens);

    // Copy relevent data for a rigid body object
    for (size_t l = 0; l < token_count; l++)
    {
        // Parse out type
        if (strcmp("active", tokens[l].key) == 0)
        {
            ret->active = (bool) tokens[l].value.n_where;
            continue;
        }

        // Parse out mass
        else if (strcmp("mass", tokens[l].key) == 0)
        {
            ret->mass = (float) atof(tokens[l].value.n_where);
            continue;
        }

        // Parse out radius
        else if (strcmp("radius", tokens[l].key) == 0)
        {
            ret->radius = (float) atof(tokens[l].value.n_where);
            continue;
        }

        // Parse out acceleration
        else if (strcmp("acceleration", tokens[l].key) == 0)
        {
            ret->acceleration = (vec3){ (float)atof(tokens[l].value.a_where[0]), (float)atof(tokens[l].value.a_where[1]), (float)atof(tokens[l].value.a_where[2]), 0.f };
            continue;
        }

        // Parse out FOV
        else if (strcmp("velocity", tokens[l].key) == 0)
        {
            ret->velocity = (vec3){ (float)atof(tokens[l].value.a_where[0]), (float)atof(tokens[l].value.a_where[1]), (float)atof(tokens[l].value.a_where[2]), 0.f };
            continue;
        }

        // Parse out angular acceleration
        else if (strcmp("angular acceleration", tokens[l].key) == 0)
        {
            ret->angular_acceleration = (quaternion){ (float)atof(tokens[l].value.a_where[0]), (float)atof(tokens[l].value.a_where[1]), (float)atof(tokens[l].value.a_where[2]), (float)atof(tokens[l].value.a_where[3]) };
            continue;
        }

        // Parse out angular velocity
        else if (strcmp("angular velocity", tokens[l].key) == 0)
        {
            ret->angular_velocity = (quaternion){ (float)atof(tokens[l].value.a_where[0]), (float)atof(tokens[l].value.a_where[1]), (float)atof(tokens[l].value.a_where[2]), (float)atof(tokens[l].value.a_where[3]) };
            continue;
        }
    }

    free(tokens);

    return ret;

    // Error handling
    {

        // Argument errors
        {
            no_token:
            #ifndef NDEBUG
                g_print_error("[G10] [Rigidbody] Null pointer provided for \"token\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}

int            update_position_and_velocity ( GXRigidbody_t *rigidbody, GXTransform_t *transform, u32 delta_time )
{

    // Argument check
    {
        #ifndef NDEBUG
            if(rigidbody == (void*)0)
                goto no_rigidbody;
            if(transform == (void*)0)
                goto no_transform;
        #endif
    }

    // Initialized data
    vec3 f = apply_force(rigidbody);
    vec3 a = mul_vec3_f(f, 1 / rigidbody->mass);

    add_vec3(&rigidbody->velocity, rigidbody->velocity, mul_vec3_f(a, delta_time / 1000.f));
    add_vec3(&transform->location, transform->location, mul_vec3_f(rigidbody->velocity, delta_time / 1000.f));

    return 0;

    // Error handling
    {
        no_rigidbody:
        #ifndef NDEBUG
            g_print_error("[G10] [Rigidbody] Null pointer provided for \"rigidbody\" in call to function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
        no_transform:
        #ifndef NDEBUG
            g_print_error("[G10] [Rigidbody] Null pointer provided for \"transform\" in call to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int            destroy_rigidbody          ( GXRigidbody_t *rigidbody )
{

    // Argument check
    {
        #ifndef NDEBUG
            if (rigidbody == (void*)0)
                goto no_rigidbody;
        #endif
    }

    rigidbody->mass         = 0;
    rigidbody->acceleration = (vec3){ 0.f,0.f,0.f };

    free(rigidbody);

    return 0;

    // Error handling
    {

        // Argument errors
        {
            no_rigidbody:
                #ifndef NDEBUG
                    g_print_error("[G10] [Rigidbody] Null pointer provided for \"rigidbody\" in call to function \"%s\"\n",__FUNCSIG__);
                #endif
                return 0;
        }
    }
}
