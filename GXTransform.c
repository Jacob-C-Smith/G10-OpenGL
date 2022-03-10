#include <G10/GXTransform.h>

// TODO: Refactor into allocator, write a constructor
GXTransform_t *create_transform       ( void )
{
    // Allocate for transform
    GXTransform_t* ret = calloc(1,sizeof(GXTransform_t));

    // Check if memory was allocated
    #ifndef NDEBUG
    if (ret == 0)
        goto no_mem;
    #endif

    // Return 
    return ret;

    // Error handling
    {
        // Standard library errors
        {
            no_mem:
            #ifndef NDEBUG
                g_print_error("[G10] [Transform] Failed to allocate memory for transform in call to \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        }
    }
}

GXTransform_t *load_transform         ( char    *path )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(path==(void *)0)
                goto no_path;
        #endif
    }

    // Uninitialized data
    u8*            data;
    GXTransform_t* ret;

    // Initialized data
    size_t         i = 0;
    FILE*          f = fopen(path, "rb");

    // Load up the file
    i    = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    ret = load_transform_as_json(data);

    // Finish up
    free(data);

    return ret;
    
    // Error handling
    {
        // Path was null pointer
        no_path:
        #ifndef NDEBUG
            g_print_error("[G10] [Transform] Null pointer provided for \"path\" in call to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXTransform_t *load_transform_as_json   ( char          *token ) 
{
    // Argument check
    {
        if ( token == (void *) 0 )
            goto no_token;
    }

    // Initialiazed data
    GXTransform_t* ret        = 0;
    size_t         len        = strlen(token),
                   token_count = parse_json(token, len, 0, 0);
    JSONToken_t*   tokens     = calloc(token_count, sizeof(JSONToken_t));
    vec3           location   = { 0,0,0 },
                   scale      = { 0,0,0 };
    quaternion     q          = { 0,0,0,0 };

    // Parse JSON Values
    parse_json(token, len, token_count, tokens);

    // Find and assign location, rotation, and scale vectors
    for (size_t k = 0; k < token_count; k++)
        if (strcmp("location", tokens[k].key) == 0)
            location = (vec3){ (float)atof(tokens[k].value.a_where[0]), (float)atof(tokens[k].value.a_where[1]), (float)atof(tokens[k].value.a_where[2]) };
        else if (strcmp("rotation", tokens[k].key) == 0)
            q        = make_quaternion_from_euler_angle((vec3) { (float)atof(tokens[k].value.a_where[0]), (float)atof(tokens[k].value.a_where[1]), (float)atof(tokens[k].value.a_where[2]) });
        else if (strcmp("quaternion", tokens[k].key) == 0)
            q        = (quaternion){ (float)atof(tokens[k].value.a_where[0]), (float)atof(tokens[k].value.a_where[1]), (float)atof(tokens[k].value.a_where[2]) , (float)atof(tokens[k].value.a_where[3]) };
        else if ( strcmp( "scale", tokens[k].key) == 0 )
            scale    = (vec3) { ( float ) atof(tokens[k].value.a_where[0] ), ( float ) atof(tokens[k].value.a_where[1] ), ( float ) atof(tokens[k].value.a_where[2] ) };
    
    // Free subcontents
    free(tokens);

    // Process transform
    return transform_from_loc_quat_sca(location, q, scale);

    // Error handling
    {
        // Argument errors
        {
            no_token:
            #ifndef NDEBUG
                g_print_error("[G10] [Transform] Null pointer provided for \"token\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        }
    }

}

GXTransform_t* transform_from_loc_quat_sca(vec3 location, quaternion rotation, vec3 scale)
{
    // Initialized data
    GXTransform_t *ret = create_transform();

    // Set location, rotation, and scale
    ret->location = location;
    ret->rotation = rotation;
    ret->scale    = scale;

    // Create a model matrix from the location, rotation, and scale
    make_model_matrix( &ret->model_matrix, ret );

    return ret;
}

void make_model_matrix(mat4 *r, GXTransform_t* transform)
{

    // Argument check
    {
        if(r ==  (void*)0)
            goto no_result;
        if (transform == (void*)0)
            goto no_transform;
    }

    mat4 sca = scale_mat4(transform->scale),
         rot = rotation_mat4_from_quaternion(transform->rotation),
         tra = translation_mat4(transform->location),
         tr  = mul_mat4_mat4(tra, sca);
        *r   = mul_mat4_mat4(rot, tr);

    return;

    // Error handling
    {
        
        // Argument errors
        {
            no_result:
            #ifndef NDEBUG
                g_print_error("[G10] [Transform] Null pointer provided for \"r\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;

            no_transform:
            #ifndef NDEBUG
                g_print_error("[G10] [Transform] Null pointer provided for \"transform\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}

int            rotate_about_quaternion ( GXTransform_t *transform, quaternion axis, float theta)
{
    // Argument check
    {
        if ( transform == (void *) 0 )
            goto no_transform;
    }

    /*
    * To rotate around a quaternion, multiply the transform
    * quaternion a special quaternion called 'p', derived by multiplying
    * u by sin( theta/2 ) and multiplying i, j, and k by cos( theta/2 ).
    * Then, multiply the inverse of p, henceforth " p' ". This is analogous
    * to complex multiplication.
    */

    float      halfAngle = theta / 2,
               cosHalf   = cosf(halfAngle),
               sinHalf   = sinf(halfAngle);
    vec3       newIJK    = { transform->rotation.i,transform->rotation.j,transform->rotation.k };
    
    rotate_vec3_by_quaternion(&newIJK, newIJK, axis);
    transform->rotation = (quaternion){ cosHalf, sinHalf * newIJK.x, sinHalf * newIJK.y, sinHalf * newIJK.z };

    return 0;

    // Error handling
    {
        no_transform:
        #ifndef NDEBUG
            g_print_error("[G10] [Transform] Null pointer provided for \"transform\" in call to funciton \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }

}

int            destroy_transform      ( GXTransform_t *transform )
{
    // Argument check
    {
        if (transform == (void*)0)
            goto noTransform;
    }

    // Free the transform
    free(transform);

    return 0;

    //  Error handling
    {
        noTransform:
        #ifndef NDEBUG
            g_print_error("[G10] [Transform] Null pointer provided for \"transform\" in call to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}
