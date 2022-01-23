#include <G10/GXCollider.h>

GXCollider_t *create_collider     ( ) 
{
	GXCollider_t* ret = calloc(1, sizeof(GXCollider_t));

	#ifndef NDEBUG
		if (ret == (void*)0)
			goto outOfMem;
	#endif

	return ret;

	// Error handling
	{
		outOfMem:
		#ifndef NDEBUG
			g_print_error("[G10] [Collider] Out of memory!\n");
		#endif
		return 0;
	}
}

GXCollider_t *load_collider       ( const char   *path ) 
{
    // Argument check
    {
        if ( path == (void*)0 )
            goto noPath;
    }

	// Uninitialized data
    u8           *data;
    GXCollider_t *ret;

    // Initialized data
    size_t        i   = 0;

    // Load up the file
    {
        i    = g_load_file(path, 0, false);
        data = calloc(i, sizeof(u8));
        g_load_file(path, data, false);
    }

    // Parse the collider as a JSON token
    ret  = load_collider_as_json(data);

    // Free heap data
    free(data);

    // Make sure the token was parsed alright
    {
        #ifndef NDEBUG
        if (ret == (void*)0)
            goto badFile;
        #endif
    }

    return ret;

    // Error handling
    {
        noPath:
            #ifndef NDEBUG
                g_print_error("[G10] [Collider] No path provided to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;

        badFile:
            #ifndef NDEBUG
                g_print_error("[G10] [Collider] There was an error parsing file \"%s\"\n", path);
            #endif
            return 0;
    }
}

GXCollider_t *load_collider_as_json ( char         *token )
{
	// Initialized data
    GXCollider_t *ret        = create_collider();
    size_t        len        = strlen(token),
                  tokenCount = parse_json(token, len, 0, (void*)0);
    JSONToken_t  *tokens     = calloc(tokenCount, sizeof(JSONToken_t));

    // Parse the collider object
    parse_json(token, len, tokenCount, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < tokenCount; j++)
    {
        // Set shape
        if (strcmp("shape", tokens[j].key) == 0)
        {
            ret->type = 0;

            if      ( strcmp("plane"      , tokens[j].value.n_where) == 0)
                ret->type = plane;
            else if ( strcmp("box"        , tokens[j].value.n_where) == 0)
                ret-> type = box;
            else if ( strcmp("sphere"     , tokens[j].value.n_where) == 0)
                ret-> type = sphere;
            else if ( strcmp("capsule"    , tokens[j].value.n_where) == 0)
                ret->type = capsule;
            else if ( strcmp("cylinder"   , tokens[j].value.n_where) == 0 )
                ret->type = cylinder;
            else if ( strcmp("cone"       , tokens[j].value.n_where) == 0 )
                ret->type = cone;
            else if ( strcmp("convex hull", tokens[j].value.n_where) == 0)
                ret->type = convexhull;

            if (ret->type == invalid)
                goto invalidShape;
            continue;
        }
        if (strcmp("dimensions", tokens[j].key) == 0)
        {
            ret->bv = create_bv();
            ret->bv->scale->x = (float)atof(tokens[j].value.a_where[0]),
            ret->bv->scale->y = (float)atof(tokens[j].value.a_where[1]),
            ret->bv->scale->z = (float)atof(tokens[j].value.a_where[2]);
            

        }
    }

    // Free heap data
    free(tokens);

    return ret;

    // Error handling
    {
        // Unable to deduce the collider shape
        invalidShape:
        #ifndef NDEBUG
            g_print_error("[G10] [Collider] Failed to determine collider type\n");
        #endif
        return 0;
    }
}

int          destroy_collider     ( GXCollider_t *collider )
{
	// TODO: Write
	return 0;
}
