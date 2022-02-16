#include <G10/GXTransform.h>

// TODO: Refactor into allocator, write a constructor
GXTransform_t *create_transform       ( void )
{
   
    // Allocate space
    GXTransform_t* ret = calloc(1,sizeof(GXTransform_t));

    // Check if memory was allocated
    if (ret == 0)
        return (void*)0;

    return ret;
}

GXTransform_t *load_transform         ( const char     path[] )
{
    // TODO: Argument check

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
    i    = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    ret = load_transform_as_json(data);

    // Finish up
    free(data);

    return ret;
    
    // Error handling
    {
        // File not found
        invalidFile:
        #ifndef NDEBUG
            g_print_error("[G10] [Transform] Failed to load file %s\n", path);
        #endif
        return 0;

        // Path was null pointer
        noArgument:
        #ifndef NDEBUG
            g_print_error("[G10] [Transform] No file supplied for path in function %s\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXTransform_t *load_transform_as_json   ( char          *token ) 
{
    // TODO: Argument check
    
    // Initialiazed data
    GXTransform_t* ret        = 0;
    size_t         len        = strlen(token),
                   tokenCount = parse_json(token, len, 0, 0);
    JSONToken_t*   tokens     = calloc(tokenCount, sizeof(JSONToken_t));
    vec3           location   = { 0,0,0 },
                   scale      = { 0,0,0 };
    quaternion     q          = { 0,0,0,0 };

    // Parse JSON Values
    parse_json(token, len, tokenCount, tokens);

    // Find and assign location, rotation, and scale vectors
    for (size_t k = 0; k < tokenCount; k++)
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
    // TODO: Error handling

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

    // Debug information
    #ifndef NDEBUG
        g_print_log("[G10] [Transform] Transform created with location (%f,%f,%f)\n" 
                  "                                         rotation (%f,%f,%f,%f)\n"
                  "                                         scale    (%f,%f,%f)\n"
                  , location.x, location.y, location.z, rotation.u, rotation.i, rotation.j, rotation.k, scale.x, scale.y, scale.z);
    #endif

    return ret;
}

void make_model_matrix(mat4 *r, GXTransform_t* transform)
{
    // TODO: Argument check
    mat4 sca = scale_mat4(transform->scale),
         rot = rotation_mat4_from_quaternion(transform->rotation),
         tra = translation_mat4(transform->location),
         ts  = mul_mat4_mat4(tra, sca);
    *r = mul_mat4_mat4(rot, ts);
    // TODO: Error handling

}

int            rotate_about_quaternion ( GXTransform_t *transform, quaternion axis, float theta)
{
    // TODO: Argument check
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
    // TODO: Error handling

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
