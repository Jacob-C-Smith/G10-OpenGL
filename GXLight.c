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

    #ifndef NDEBUG
        g_print_log("[G10] [Light] Loading \"%s\".\n", (char*)path);
    #endif	

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
    GXLight_t*   ret        = create_light();
    size_t       len        = strlen(token);
    size_t       token_count = parse_json(token, len, 0, (void*)0);
    JSONToken_t* tokens     = calloc(token_count, sizeof(JSONToken_t));

    // Parse the camera object
    parse_json(token, len, token_count, tokens);

    // Iterate through key / value pairs to find relevent information
    for (size_t l = 0; l < token_count; l++)
    {
        // Parse out the light name
        if (strcmp("name", tokens[l].key) == 0)
        {
            size_t len = strlen(tokens[l].value.n_where);
            ret->name = calloc(len + 1, sizeof(u8));

            strncpy(ret->name, (const char*)tokens[l].value.n_where,len);
            continue;
        }

        // Parse out light color
        if (strcmp("color", tokens[l].key) == 0)
        {
            ret->color = (vec3){ (float)atof(tokens[l].value.a_where[0]), (float)atof(tokens[l].value.a_where[1]), (float)atof(tokens[l].value.a_where[2]) };
            continue;
        }

        // Parse out light position
        if (strcmp("location", tokens[l].key) == 0)
        {
            ret->location = (vec3){ (float)atof(tokens[l].value.a_where[0]), (float)atof(tokens[l].value.a_where[1]), (float)atof(tokens[l].value.a_where[2]) };
            continue;
        }
    }

    free(tokens);
    ret->next   = 0;

    return ret;

    // Error handling
    {

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