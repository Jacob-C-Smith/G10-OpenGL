#include <G10/GXPart.h>

#include <G10/GXPLY.h>

GXPart_t *create_part     ( void )
{
    // Initialized data
    GXPart_t* ret = calloc(1,sizeof(GXPart_t));

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
            g_print_error("[G10] [Part] Out of memory.\n");
        #endif
        return 0;
    }
}

GXPart_t *load_part       ( const char path[] )
{

    // Argument check
    {
        if(path == (void *)0)
            goto no_path;
    }

    // Uninitialized data
    GXPart_t    *ret;
    size_t       i;
    char        *data;
    int          token_count;
    JSONToken_t* tokens;

    // Initialized data
    size_t    l   = 0;

    // Load up the file
    i    = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    ret = load_part_as_json(data);

    free(data);

    return ret;

    // Error handling
    {

        // Argument errors
        {
            no_path:
            #ifndef NDEBUG
                g_print_error("[G10] [Part] Null pointer provided for \"path\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}

GXPart_t *load_part_as_json ( char      *token )
{
    // Argument check
    {
        #ifndef NDEBUG
            if (token == 0)
                goto noToken;
        #endif
    }

    // Initialized data
    GXPart_t    *ret         = create_part();
    size_t       len         = strlen(token),
                 token_count = parse_json(token, len, 0, (void*)0);
    JSONToken_t *tokens      = calloc(token_count, sizeof(JSONToken_t));

    char        *name        = 0,
                *path        = 0,
                *material    = 0;

    // Error checking
    {
        #ifndef NDEBUG
            if(tokens == (void *)0)
                goto no_mem;
        #endif
    }

    // Parse the part object
    parse_json(token, len, token_count, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < token_count; j++)
    {

        // Handle comments
        if (strcmp("name", tokens[j].key) == 0)
        {
            if (tokens[j].type == JSONstring)
                name = tokens[j].value.n_where;
            else
                goto name_type_error;

            continue;
        }

        // Load from a path
        else if (strcmp("path", tokens[j].key) == 0)
        {
            if (tokens[j].type == JSONstring)
                path = tokens[j].value.n_where;
            else
                goto path_type_error;

            continue;
        }

        // Set material
        else if (strcmp("material", tokens[j].key) == 0)
        {
            if (tokens[j].type == JSONstring)
                material = tokens[j].value.n_where;
            else
                goto material_type_error;

            continue;
        }
    }

    // Check the cache for the part
    {
        GXPart_t* p_ret = g_find_part(g_get_active_instance(), name);

        if (p_ret)
        {
            destroy_part(ret);
            ret = p_ret;
            g_print_log("[G10] [Part] Part \"%s\" loaded from cache\n", name);

            goto exit_cache;
        }
    }

    // Construct the part
    {

        // Set and copy the part name
        {

            // Initialized data
            char*  path    = tokens[j].value.n_where;
            size_t pathLen = strlen(path);

            // Copy the string
            ret->name = calloc(pathLen+1, sizeof(char));

            // Error checking
            {
                if ( ret->name == (void *)0)
                    goto no_mem;
            }

            strncpy(ret->name, path, pathLen);
        }

        // Load the part
        load_ply(path, ret);

        // Set and copy the material name
        {
            size_t material_name_len = strlen(material);
            ret->material = calloc(1, material_name_len + 1);
            strncpy(ret->material, material, material_name_len);
        }
    }

    g_cache_part(g_get_active_instance(), ret);
    exit_cache:
    ret->users++;
     
    free(tokens);

    //ret->next = 0;

    return ret;

    // Error handling
    {

        // Argument errors
        {
            noToken:
            #ifndef NDEBUG
                g_print_error("[G10] [Part] Null pointer provided for \"token\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        }
    }
}

GXPart_t* duplicate_part(GXPart_t* part)
{
    // Argument Check
    {
        #ifndef NDEBUG
            if(part == (void *)0)
                goto null_part;
            if (part->users == -1)
                goto dupe_dupe;
        #endif
    }

    // Initialized data
    GXPart_t *ret = create_part();

    // Copy constant data
    ret->vertex_groups  = part->vertex_groups;

    ret->vertex_array   = part->vertex_array;
    ret->vertex_buffer  = part->vertex_buffer;
    ret->element_buffer = part->element_buffer;

    // Set users to -1, as to indicate this is a duplicate
    ret->users          = -1;

    // Increment users
    part->users++;

    return ret;

    // Error handling
    {
        null_part:
            #ifndef NDEBUG
                g_print_error("[G10] [Part] Null pointer provided for \"part\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        dupe_dupe:
            #ifndef NDEBUG
                g_print_error("[G10] [Part] Can not duplicate a duplicate. Please use the original part\n", __FUNCSIG__);
            #endif
            return 0;
    }
}

GXPart_t *get_part        ( GXPart_t  *head, const char name[] )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if (name == 0)
                goto nullName;
        #endif  
    }

    // Create a pointer to the head of the list
    GXPart_t* i = head;

    // Check to see if there are even any parts in the first place
    if (i == 0)
        goto noParts; 

    // Iterate through parts list 
    while (i)
    {

        // Correct part?
        if (strcmp(name, i->name) == 0)
            return i; // Return a pointer

        // Iterate
        i = i->next;
    }

    // Unable to locate part
    goto noMatch;

    // Error handling
    {

        // There are no parts
        noParts:
        #ifndef NDEBUG
            g_print_error("[G10] [Mesh] There are no parts.\n");
        #endif
        return 0;

        // There is no matching part
        noMatch:
        #ifndef NDEBUG
            g_print_error("[G10] [Mesh] There is no part named \"%s\".\n", name);
        #endif
        return 0;

        // Argument errors
        {
            nullName:
            #ifndef NDEBUG
                g_print_error("[G10] [Mesh] Null pointer provided to \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}

int       append_part     ( GXPart_t  *head, GXPart_t  *part )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if(head == 0)
                goto no_head;
            if(part == 0)
                goto nullPart;
        #endif
    }

    // Set the pointer to the head of the linked list
    GXPart_t* i = head;

    // Check if the head is null. If so, set the head to the mesh
    if (i == 0)
    {
        head = part;
        return 0;
    }

    // Search for the end of the linked list, and check every mesh 
    while (i->next)
    {
        // Error checking

        if (i == part)
            goto already_here;

        if (strcmp(i->name, part->name) == 0)
            goto duplicateName;
        i = i->next;
    }

    // Assign next as entity
    i->next = part;
    
    already_here:;
    return 0;

    // Error handling
    {
        // Two parts with the same name cannot exist in the same mesh
        duplicateName:
        #ifndef NDEBUG
            g_print_error("[G10] [Mesh] Part \"%s\" can not be appended because a part with that name already exists\n", part->name);
        #endif
        return 0;

        // The mesh argument was null
        nullMesh:
        #ifndef NDEBUG
            g_print_error("[G10] [Mesh] Null pointer provided for \"head\" in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;

        // Argument errors
        {
            no_head:
            #ifndef NDEBUG
                g_print_error("[G10] [Mesh] Null pointer provided for \"head\" in function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
            
            nullPart:
            #ifndef NDEBUG
                g_print_error("[G10] [Mesh] Null pointer provided for \"part\" in function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }
    }
}

int       draw_part       ( GXPart_t  *part )
{
    // Argument check
    {
        if (part == (void*)0)
            goto noPart;
    }

    // Draw the part
    glBindVertexArray(part->vertex_array);
    glDrawElements(GL_TRIANGLES, part->elements_in_buffer, GL_UNSIGNED_INT, 0);

    return 0;

    // Error handling
    {

        // Argument Check
        {
            noPart:
            #ifndef NDEBUG
                g_print_error("[G10] [Mesh] Null pointer provided for parameter \"part\" in function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        }
    }
}
 
GXPart_t *remove_part     ( GXPart_t  *head, const char name[] )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(name == (void*)0)
                goto noName;
        #endif
    }

    // Initialized data
    GXPart_t *i   = head,
             *ret = 0;

    // Quick sanity check
    if (i == 0)
        goto noHead;

    // Check the head
    if (strcmp(name, i->name) == 0)
    {
        GXPart_t* j = i->next;
        destroy_part(i);
        i = j;
        return 0;
    }

    // Destroy the named part
    while (i->next)
    {
        if (strcmp(name, i->next->name) == 0)
        {
            // Make a copy of the part that is 2 positions ahead of the current part
            GXPart_t* j = i->next->next;

            // Delete the part
            ret = i->next;

            // Stitch up the linked list 
            i->next = j;

            return ret;
        }
        i = i->next;
    }
    goto noMatch;


    // Error handling
    {

        // Can't find a part with that name.
        noMatch:
        #ifndef NDEBUG
            g_print_error("[G10] [Mesh] There is no part  named \"%s\".\n", name);
        #endif
        return 0;

        // No head was supplied
        noHead:
        #ifndef NDEBUG
            g_print_error("[G10] [Mesh] Null pointer provided for parameter \"head\" in function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;

        // Argument errors
        {
            noName:
            #ifndef NDEBUG
                g_print_error("[G10] [Mesh] Null pointer provided for parameter \"name\" in function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        }
    }
}

int       destroy_part    ( GXPart_t  *part )
{

    // Argument check
    {
        #ifndef NDEBUG
            
            // Null pointer for part?
            if(part == 0)
                goto nullPart;

            // Part in use?
            if (part->users > 1)
                goto in_use;

            // Part is duplicate?
            if (part->users == -1)
                goto dupe;
        #endif
    }

    // Free the OpenGL buffers
    glDeleteVertexArrays(1, &part->vertex_array);
    glDeleteBuffers(1, &part->vertex_buffer);
    glDeleteBuffers(1, &part->element_buffer);

    dupe:

    // Free the part, name, and material
    free(part->name);
    free(part->material);

    free(part);

    return 0;

    // Error handling
    {

        // Argument check
        {
            nullPart:
                #ifndef NDEBUG 
                    g_print_error("[G10] [Mesh] Null pointer provided for part in function \"%s\"\n", __FUNCSIG__);
                #endif
            return 0;
            in_use:
                part->users--;
                return 0;
        }
    }
}

