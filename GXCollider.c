#include <G10/GXCollider.h>

GXCollider_t *create_collider     ( ) 
{
	GXCollider_t* ret = calloc(1, sizeof(GXCollider_t));

    // Check memory
    {
	    #ifndef NDEBUG
		    if (ret == (void*)0)
		    	goto outOfMem;
	    #endif
    }

	return ret;

	// Error handling
	{

        // Standard library errors
        {
		    outOfMem:
		    #ifndef NDEBUG
		    	g_print_error("[G10] [Collider] Out of memory!\n");
	    	#endif
    		return 0;
        }
	}
}

GXCollider_t *load_collider       ( const char   *path ) 
{
    
    // Argument check
    {
        #ifndef NDEBUG
            if ( path == (void*)0 )
                goto noPath;
        #endif
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

        // Argument errors
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
}

GXCollider_t *load_collider_as_json ( char         *token )
{

    // Argument check
    {
        #ifndef NDEBUG
            if(token==(void*)0)
                goto no_token;
        #endif
    }

	// Initialized data
    GXCollider_t *ret              = create_collider();
    size_t        len              = strlen(token),
                  token_count      = parse_json(token, len, 0, (void*)0),
                  i                = 0;
    JSONToken_t  *tokens           = calloc(token_count, sizeof(JSONToken_t));

    char         *type             = 0,
                **dimensions       = 0,
                 *convex_hull_path = 0;

    // Parse the collider object
    parse_json(token, len, token_count, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < token_count; j++)
    {

        // Parse the type
        if ( strcmp("type"            , tokens[j].key) == 0 )
        {
            if (tokens[j].type == JSONstring)
                type = tokens[j].value.n_where;
            else
                goto type_type_error;

            continue;
        }

        // Parse the dimensions
        if ( strcmp("dimensions"      , tokens[j].key) == 0 )
        { 
            if(tokens[j].type == JSONarray)
                dimensions = tokens[j].value.a_where;
            else
                goto dimensions_type_error;
            
            continue;
        }

        // Parse the convex hull path
        if ( strcmp("convex hull path", tokens[j].key) == 0 )
        {
            if (tokens[j].type == JSONstring)
                convex_hull_path = tokens[j].value.n_where;
            else
                goto convex_hull_path_type_error;

            continue;
        }

        loop:
    }

    // Construct the collider
    {

        // Set the type
        {
            if (type)
            {
                if      ( strcmp("plane"      , type) == 0 )
                    ret->type = quad;
                else if ( strcmp("box"        , type) == 0 )
                    ret-> type = box;
                else if ( strcmp("sphere"     , type) == 0 )
                    ret-> type = sphere;
                else if ( strcmp("capsule"    , type) == 0 )
                    ret->type = capsule;
                else if ( strcmp("cylinder"   , type) == 0 )
                    ret->type = cylinder;
                else if ( strcmp("cone"       , type) == 0 )
                    ret->type = cone;
                else if ( strcmp("convex hull", type) == 0 )
                    ret->type = convexhull;
            }
            else
                goto no_type;
        }

        // Set the dimensions
        {
            if (dimensions)
            {
                
                ret->bv           = create_bv();
                if(ret->bv->entity)
                {
                    if(ret->bv->entity->transform)
                        ret->bv->dimensions = &ret->bv->entity->transform->scale;
                }
                else
                {
                    ret->bv->dimensions = calloc(1, sizeof(vec3));
                }
        
                ret->bv->dimensions->x = (float)atof(dimensions[0]) / 2.f,
                ret->bv->dimensions->y = (float)atof(dimensions[1]) / 2.f,
                ret->bv->dimensions->z = (float)atof(dimensions[2]) / 2.f;
            
            }
            else
                goto no_dimensions;
        }

        // Load and set the convex hull
        {
            if (convex_hull_path)
                ret->convex_hull = load_ply_geometric_points(convex_hull_path, ret->convex_hull_count);
        }
    }

    // Free heap data
    free(tokens);

    return ret;

    // Error handling
    {
        
        // Argument errors
        {
            no_token:
            #ifndef NDEBUG
                g_print_error("[G10] [Collider] Null pointer provided for \"token\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }

        // Data errors
        {
            invalidShape:
            #ifndef NDEBUG
                g_print_error("[G10] [Collider] Failed to determine collider type\n");
            #endif
            return 0;
        }

    }
}

int add_start_collision_callback  ( GXCollider_t *collider, void* function_pointer )
{
    // Argument check
    {
        if (collider == (void*)0)
            goto no_collider;
        if (function_pointer == (void*)0)
            goto noFunPtr;
    }

    if (collider->aabb_start_callbacks == 0)
    {
        collider->aabb_callback_max = 2,
        collider->aabb_start_callbacks = calloc(collider->aabb_start_callback_max, sizeof(void*));
    }

    if (collider->aabb_start_callback_count + 1 > collider->aabb_start_callback_max)
    {
        collider->aabb_callback_max *= 2;
        void **callbacks    = calloc(collider->aabb_callback_max, sizeof(void*)),
              *t            = collider->aabb_start_callbacks;

        memcpy(callbacks, collider->aabb_start_callbacks, collider->aabb_start_callback_count * sizeof(void*));
        collider->aabb_start_callbacks = callbacks;

        free(t);
    }

    collider->aabb_start_callbacks[collider->aabb_start_callback_count++] = function_pointer;

    return 0;

    // Error handling
    {
        no_collider:
        #ifndef NDEBUG
            g_print_error("[G10] [Collider] Null pointer provided for \"collider\" in call to funciton \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;

        noFunPtr:
        #ifndef NDEBUG
            g_print_warning("[G10] [Collider] Null pointer provided for \"function_pointer\" in call to funciton \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }
}

int add_collision_callback        ( GXCollider_t *collider, void* function_pointer )
{

    // Argument check
    {
        if (collider == (void*)0)
            goto no_collider;
        if (function_pointer == (void*)0)
            goto noFunPtr;
    }

    if (collider->aabb_callbacks == 0)
    {
        collider->aabb_callback_max = 2,
            collider->aabb_callbacks = calloc(collider->aabb_callback_max, sizeof(void*));
    }

    if (collider->aabb_callback_count + 1 > collider->aabb_callback_max)
    {
        collider->aabb_callback_max *= 2;
        void **callbacks    = calloc(collider->aabb_callback_max, sizeof(void*)),
              *t            = collider->aabb_callbacks;

        memcpy(callbacks, collider->aabb_callbacks, collider->aabb_callback_count * sizeof(void*));
        collider->aabb_callbacks = callbacks;

        free(t);
    }

    collider->aabb_callbacks[collider->aabb_callback_count++] = function_pointer;

    return 0;

    // Error handling
    {
        no_collider:
        #ifndef NDEBUG
            g_print_error("[G10] [Collider] Null pointer provided for \"collider\" in call to funciton \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;

        noFunPtr:
        #ifndef NDEBUG
            g_print_warning("[G10] [Collider] Null pointer provided for \"function_pointer\" in call to funciton \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }
}

int add_end_collision_callback    ( GXCollider_t *collider, void* function_pointer )
{
    // Argument check
    {
        if (collider == (void*)0)
            goto no_collider;
        if (function_pointer == (void*)0)
            goto noFunPtr;
    }

    if (collider->aabb_end_callbacks == 0)
    {
        collider->aabb_end_callback_max = 2,
        collider->aabb_end_callbacks = calloc(collider->aabb_end_callback_max, sizeof(void*));
    }

    if (collider->aabb_end_callback_count + 1 > collider->aabb_end_callback_max)
    {
        collider->aabb_end_callback_max *= 2;
        void **callbacks    = calloc(collider->aabb_end_callback_max, sizeof(void*)),
              *t            = collider->aabb_end_callbacks;

        memcpy(callbacks, collider->aabb_end_callbacks, collider->aabb_end_callback_count * sizeof(void*));
        collider->aabb_end_callbacks = callbacks;

        free(t);
    }

    collider->aabb_end_callbacks[collider->aabb_end_callback_count++] = function_pointer;

    return 0;

    // Error handling
    {
        no_collider:
        #ifndef NDEBUG
            g_print_error("[G10] [Collider] Null pointer provided for \"collider\" in call to funciton \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;

        noFunPtr:
        #ifndef NDEBUG
            g_print_warning("[G10] [Collider] Null pointer provided for \"function_pointer\" in call to funciton \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }
}

int          destroy_collider     ( GXCollider_t *collider )
{
	// Argument check
    {
        if (collider == (void*)0)
            goto no_collider;
    }

	return 0;

    // Error handling
    {

        // Argument errors
        {
            no_collider:
            #ifndef NDEBUG
                g_print_error("[G10] [Collider] Null pointer provided for \"collider\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}
