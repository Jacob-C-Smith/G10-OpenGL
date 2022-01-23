#include <G10/GXLight.h>

GXLight_t *create_light     ( )
{
    // Initialized data
    GXLight_t* ret = calloc(1,sizeof(GXLight_t));

    if (ret == (void*)0)
        return ret;

    // Set everything to zero
    ret->name = (void*)0;

    ret->color.x    = 0.f;
    ret->color.y    = 0.f;
    ret->color.z    = 0.f;
    ret->color.w    = 0.f;

    ret->location.x = 0.f;
    ret->location.y = 0.f;
    ret->location.z = 0.f;
    ret->location.w = 0.f;

    ret->next       = 0;

    // Return
    return ret;
}

GXLight_t *load_light       ( const char path[] )
{
    // Uninitialized data
    u8*        data;
    GXLight_t* ret;

    // Initialized data
    size_t       i = 0;
    FILE*        f = fopen(path, "rb");

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
}

GXLight_t *load_light_as_json ( char      *token )
{
    // Initialized data
    GXLight_t*   ret        = create_light();
    size_t       len        = strlen(token);
    size_t       tokenCount = parse_json(token, len, 0, (void*)0);
    JSONToken_t* tokens     = calloc(tokenCount, sizeof(JSONToken_t));

    // Parse the camera object
    parse_json(token, len, tokenCount, tokens);

    // Iterate through key / value pairs to find relevent information
    for (size_t l = 0; l < tokenCount; l++)
    {
        // Parse out the light name
        if (strcmp("name", tokens[l].key) == 0)
        {
            ret->name = calloc(strlen(tokens[l].value.n_where)+1,sizeof(u8));
            if (ret->name == (void*)0)
                return 0;
            strcpy(ret->name, (const char*)tokens[l].value.n_where);
            continue;
        }

        // Parse out light color
        if (strcmp("color", tokens[l].key) == 0)
        {
            ret->color = (vec3){ (float)atof(tokens[l].value.a_where[0]), (float)atof(tokens[l].value.a_where[1]), (float)atof(tokens[l].value.a_where[2]) };
            continue;
        }

        // Parse out light position
        if (strcmp("position", tokens[l].key) == 0)
        {
            ret->location = (vec3){ (float)atof(tokens[l].value.a_where[0]), (float)atof(tokens[l].value.a_where[1]), (float)atof(tokens[l].value.a_where[2]) };
            continue;
        }
    }

    free(tokens);
    ret->next   = 0;

    return ret;
}

int        destroy_light    ( GXLight_t *light )
{
    free((void*)light->name);

    light->color.x    = 0.f,
    light->color.y    = 0.f,
    light->color.z    = 0.f;

    light->location.x = 0.f,
    light->location.y = 0.f,
    light->location.z = 0.f;

    light->next = 0;

    free(light);

    return 0;
}