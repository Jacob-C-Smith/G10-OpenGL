#include <G10/GXMaterial.h>

GXTexture_t *missingTexture;

GXMaterial_t *create_material     ( )
{
    // Initialized data
    GXMaterial_t* ret = calloc(1,sizeof(GXMaterial_t));

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
            g_print_error("[G10] [Material] Out of memory.\n");
        #endif
        return 0;
    }
}

GXMaterial_t *load_material       ( const char     path[] )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(path==0)
                goto noPath;
        #endif
    }

    // Uninitialized data
    u8*          data;
    size_t       i;
    GXMaterial_t* ret;
    
    // Load up the file
    i = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    ret = load_material_as_json(data);

    free(data);

    // Return the material
    return ret;

    // Error handling
    {
        noPath:
        #ifndef NDEBUG
            g_print_log("[G10] [Material] Null pointer provided for \"path\" in call to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXMaterial_t *load_material_as_json ( char         *token )
{
    // Argument check
    {
        if (token == (void*)0)
            goto no_token;
    }

    // Uninitialized data
    size_t        len,
                  rootToken_count;
    JSONToken_t  *tokens;

    // Initialized data
    GXMaterial_t *ret = create_material();

    char         *name   = 0, 
                 *albedo = 0,
                 *normal = 0,
                 *metal  = 0,
                 *rough  = 0,
                 *ao     = 0,
                 *height = 0;

    // Parse JSON Values
    {
        len = strlen(token), rootToken_count = parse_json(token, len, 0, 0);
        tokens = calloc(rootToken_count, sizeof(JSONToken_t));

        // Error checking
        {
            #ifndef NDEBUG
                if (tokens == (void*)0)
                    return no_mem;
            #endif
        }

        parse_json(token, len, rootToken_count, tokens);
    }

    // TODO: Write code to load many albedos     

    // Find and load the textures
    for (size_t k = 0; k < rootToken_count; k++)
    {
        if      ( strcmp("name"  , tokens[k].key) == 0)
        {
            // TODO: FIX
            name = tokens[k].value.n_where;
            GXMaterial_t *p_ret = g_find_material(g_get_active_instance(), tokens[k].value.n_where);
            
            if(p_ret)
            { 
                destroy_material(ret);
                ret = p_ret;
                #ifndef NDEBUG
                    g_print_log("[G10] [Material] Material \"%s\" loaded from cache\n", p_ret->name);
                #endif
                goto exit_cache;
            }


            size_t nameLen = strlen(tokens[k].value.n_where);
            ret->name = calloc(nameLen + 1, sizeof(u8));
            if (ret->name == (void*)0)
                return 0;
            strncpy(ret->name, tokens[k].value.n_where, nameLen);

            continue;
        }
        else if ( strcmp("albedo", tokens[k].key) == 0)
        {
            if (tokens[k].type == JSONstring)
                albedo = tokens[k].value.n_where;
            else
                goto albedo_type_error;

            continue;
        }
        else if ( strcmp("normal", tokens[k].key) == 0)
        {
            if (tokens[k].type == JSONstring)
                normal = tokens[k].value.n_where;
            else
                goto normal_type_error;

            continue;
        }
        else if ( strcmp("metal" , tokens[k].key) == 0)
        {
            if (tokens[k].type == JSONstring)
                metal = tokens[k].value.n_where;
            else
                goto metal_type_error;

            continue;
        }
        else if ( strcmp("rough" , tokens[k].key) == 0)
        {
            
            if (tokens[k].type == JSONstring)
                rough = tokens[k].value.n_where;
            else
                goto rough_type_error;

            continue;
        }
        else if ( strcmp("ao"    , tokens[k].key) == 0)
        {
            
            if (tokens[k].type == JSONstring)
                ao = tokens[k].value.n_where;
            else
                goto ao_type_error;

            continue;
        }
        else if ( strcmp("height", tokens[k].key) == 0)
        {
            
            if (tokens[k].type == JSONstring)
                height = tokens[k].value.n_where;
            else
                goto height_type_error;

            continue;
        }
    }
    
    // Construct the material
    {
        // Set the name
        {

        }

        // Load and set the albedo
        {
            if (albedo)
                ret->albedo = load_texture(albedo);
        }

        // Load and set the normal
        {
            if (normal)
                ret->normal = load_texture(normal);
        }

        // Load and set the metal
        {
            if (metal)
                ret->metal = load_texture(metal);
        }

        // Load and set the rough
        {
            if (rough)
                ret->rough = load_texture(rough);
        }

        // Load and set the ao
        {
            if (ao)
                ret->ao = load_texture(ao);
        }

        // Load and set the height
        {
            if (height)
                ret->height = load_texture(height);
        }


    }

    g_cache_material(g_get_active_instance(), ret);
    exit_cache:
    ret->users++;

    // Free root contents
    free(tokens);

    return ret;

    // Error handling
    {

        // Argument check
        {
            no_token:
            #ifndef NDEBUG
                g_print_error("[G10] [Material] Null pointer provided for \"token\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif 
            return 0;
        }
    }
}

GXMaterial_t* duplicate_material(GXMaterial_t* material)
{
    // Argument Check
    {
        #ifndef NDEBUG
            if(material == (void *)0)
                goto null_material;
            if (material->users == -1)
                goto dupe_dupe;
        #endif
    }

    // Initialized data
    GXMaterial_t *ret = create_material();

    // Copy constant data
    ret->albedo = material->albedo;
    ret->rough  = material->rough;
    ret->metal  = material->metal;
    ret->normal = material->normal;
    ret->ao     = material->ao;

    ret->name   = calloc(strlen(material->name)+10, sizeof(u8));

    sprintf(ret->name, "%s (%d)", material->name, ret->users);

    // Set users to -1, as to indicate this is a duplicate
    ret->users          = -1;

    // Increment users
    ret->users++;

    return ret;

    // Error handling
    {
        null_material:
            #ifndef NDEBUG
                g_print_error("[G10] [Material] Null pointer provided for \"material\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        dupe_dupe:
            #ifndef NDEBUG
                g_print_error("[G10] [Material] Can not duplicate a duplicate. Please use the original material\n", __FUNCSIG__);
            #endif
            return 0;
    }
}

GXMaterial_t * get_material( GXMaterial_t *materials, const char name[] )
{

    // Argument checking
    {
        #ifndef NDEBUG
            if (materials == 0)
                goto nullMaterials;
            if (name == 0)
                goto nullName;
        #endif
    }

    // Initialized data
    GXMaterial_t* i = materials;

    // Sanity check
    if (i == 0)
        goto noMaterial;
    
    // Iterate through materials list
    while (i)
    {
        // Correct material?
        if (strcmp(name, i->name) == 0)
            return i; // Return a pointer

        // Iterate
        i = i->next;
    }
    
    // Unable to locate material
    goto noMatch;

    // Error handling
    {
        // There are no materials
        noMaterial:
        #ifndef NDEBUG
            g_print_error("[G10] [Material] There are no materials.\n");
        #endif
        return 0;	

        // There is no match
        noMatch:
        #ifndef NDEBUG
            g_print_error("[G10] [Material] There is no materials named \"%s\".", name);
        #endif
        return 0;
    
        // Argument errors
        {
        
            nullMaterials:
            #ifndef NDEBUG
                g_print_error("[G10] [Material] Null pointer provided for material in function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
    
            
            nullName:
            #ifndef NDEBUG
                g_print_error("[G10] [Material] Null pointer provided for name in function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}

int           append_material     ( GXMaterial_t *head     , GXMaterial_t *material )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if(material == 0)
                goto nullMaterial;
        #endif
    }

    // Set the pointer to the head of the linked list
    GXMaterial_t* i = head;

    // Check if the head is null. If so, set the head to the mesh
    if (i == 0)
    {
        head = material;
        return 0;
    }

    // Search for the end of the linked list, and check every material 
    while (i->next)
    {

        if (i == material)
            goto already_here;

        // Error checking
        if (strcmp(i->name, material->name) == 0)
            goto duplicateName;



        i = i->next;
    }

    // Assign next as entity
    i->next = material;

    already_here:;

    return 0;

    // Error handling
    {
        // Two materials with the same name cannot exist in the same list
        duplicateName:
        #ifndef NDEBUG
            g_print_warning("[G10] [Material] Material \"%s\" can not be appended because a material with that name already exists\n", material->name);
        #endif
        return 0;

        // Argument errors
        {
            nullHead:
            #ifndef NDEBUG
                g_print_error("[G10] [Material] Null pointer provided for \"head\" in function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;

            // The material argument was null
            nullMaterial:
            #ifndef NDEBUG
                g_print_error("[G10] [Material] Null pointer provided for \"part\" in function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}


int           destroy_material     ( GXMaterial_t *material )
{

    // Argument check
    {
        if (material == (void*)0)
            goto no_material;
        if (material->users > 1)
            goto in_use;
    }

    // Unload all of the textures
    if(material->albedo)
        unload_texture(material->albedo);
    if(material->normal)
        unload_texture(material->normal);
    if(material->metal)
        unload_texture(material->metal);
    if(material->rough)
        unload_texture(material->rough);
    if(material->ao)
        unload_texture(material->ao);

    free(material->name);

    // Free the material
    free(material);

    return 0;

    // Error handling
    {

        // Argument errors
        {
            no_material:
            #ifndef NDEBUG
                g_print_error("[G10] [Material] Null pointer provided for \"material\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }

        in_use:
            material->users--;
            return 0;
    }
}
