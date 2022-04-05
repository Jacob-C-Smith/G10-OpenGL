#include <G10/GXLight.h>

GXLight_t *create_light     ( )
{
    // Initialized data
    GXLight_t* ret = calloc(1,sizeof(GXLight_t));

    // Check memory
    {
        #ifndef NDEBUG
            if (ret == (void*)0)
                goto no_mem;
        #endif 
    }

    // Return
    return ret;

    // Error handling
    {
        no_mem:
        #ifndef NDEBUG
            g_print_error("[G10] [Light] Unable to allocate memory in call to function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }
}

GXLight_t *load_light       ( const char path[] )
{

    // Argument check
    {
        if (path == (void*)0)
            goto no_path;
    }

    // Uninitialized data
    u8*        data;
    GXLight_t* ret;

    // Initialized data
    size_t       i = 0;

    // Load up the file
    i = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    ret = load_light_as_json(data);

    // Finish up
    free(data);

    return ret;

    // Error handling
    {

        // Argument errors
        {
            no_path:
            #ifndef NDEBUG
                g_print_error("[G10] [Light] Null pointer provided \"path\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}

GXLight_t *load_light_as_json ( char      *token )
{

    // Argument check
    {
        if (token == (void*)0)
            goto no_token;
    }

    // Initialized data
    GXLight_t    *ret         = create_light();
    size_t        len         = strlen(token);
    size_t        token_count = parse_json(token, len, 0, (void*)0);
    JSONToken_t  *tokens      = calloc(token_count, sizeof(JSONToken_t));

    char         *name        = 0,
                **color       = 0, 
                **location    = 0; 

    // Error checking
    {
        #ifndef NDEBUG
            if(tokens == (void *)0)
                goto no_mem;
        #endif
    }

    // Parse the camera object
    parse_json(token, len, token_count, tokens);

    // Iterate through key / value pairs to find relevent information
    for (size_t i = 0; i < token_count; i++)
    {

        // Parse out the light name
        if (strcmp("name", tokens[i].key) == 0)
        {

            // Type check
            if (tokens[i].type == JSONstring)
                name = tokens[i].value.n_where;
            else
                goto name_type_error;

            continue;
        }

        // Parse out light color
        if (strcmp("color", tokens[i].key) == 0)
        {

            // Type check
            if (tokens[i].type == JSONarray)
                color = tokens[i].value.a_where;
            else
                goto color_type_error;

            continue;
        }

        // Parse out light position
        if (strcmp("location", tokens[i].key) == 0)
        {

            // Type check
            if (tokens[i].type == JSONarray)
                location = tokens[i].value.a_where;
            else
                goto location_type_error;

            continue;
        }
    }

    // Construct the light
    {

        // Set and copy the name
        {
            size_t name_len = strlen(name);
            ret->name = calloc(name_len + 1, sizeof(u8));

            // Error checking
            {
                #ifndef NDEBUG
                    if(ret->name == (void *)0)
                        goto no_mem;
                #endif
            }

            strncpy(ret->name, (const char*)name, name_len);
        }

        // Set the color
        {
            ret->color = (vec3){ (float)atof(color[0]), (float)atof(color[1]), (float)atof(color[2]) };
        }


        // Set the location
        {
            ret->location = (vec3){ (float)atof(location[0]), (float)atof(location[1]), (float)atof(location[2]) };
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
                g_print_error("[Standard Library] Failed to allocate memory in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;

        }

        // Argument errors
        {
            no_token:
            #ifndef NDEBUG
                g_print_error("[G10] [Light] Null pointer provided for \"token\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif 
            return 0;
        }
    }
}

int        destroy_light    ( GXLight_t *light )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(light==(void *)0)
                goto noLight;
        #endif
    }

    free(light->name);

    free(light);

    return 0;

    // Error handling
    {
        // Argument errors
        {
            noLight:
            #ifndef NDEBUG
                g_print_error("[G10] [Light] Null pointer provided for \"light\" in call to \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}