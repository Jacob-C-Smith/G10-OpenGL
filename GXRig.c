#include <G10/GXRig.h>

GXRig_t  *create_rig          ( )
{
    GXRig_t* ret = calloc(1,sizeof(GXBone_t));

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
            g_print_error("[G10] [Rig] Out of memory.\n");
        #endif
        return 0;
    }
}

GXPose_t* create_pose()
{
    GXPose_t* ret = calloc(1, sizeof(GXPose_t));

    // Check the memory
    #ifndef NDEBUG
        if(ret==0)
            goto noMem;
    #endif

    return ret;

    // Error handling
    {
        noMem:
        #ifndef NDEBUG
            g_print_error("[G10] [Rig] Out of memory.\n");
        #endif
        return 0;
    }
}

GXBone_t *create_bone         ( )
{
    // Allocate space
	GXBone_t* ret       = calloc(1,sizeof(GXBone_t));
    
    // Check the memory
    #ifndef NDEBUG
        if (ret == 0)
            goto noMem;
    #endif

    ret->head           = calloc(1,sizeof(vec3));
    ret->tail           = calloc(1,sizeof(vec3));
    ret->transformation = calloc(1,sizeof(mat4));
    
    return ret;

    // Error handling
    {
        noMem:
        #ifndef NDEBUG
            g_print_error("[G10] [Rig] Out of memory.\n");
        #endif
        return 0;
    }
}

GXRig_t  *load_rig            ( const char *path )
{
    // Argument Check
    {
        if (path == 0)
            return (void*)0;
    }

    // Uninitialized data
    size_t       i;
    char        *data;
    int          token_count;
    JSONToken_t *tokens;
    GXRig_t     *ret;

    // Initialized data
    FILE        *f = fopen(path, "rb");

    // Load the file
    i    = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);
    
    ret = load_rig_as_json(data);

    free(data);

    return ret;
}

GXRig_t  *load_rig_as_json      ( char       *token )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(token == 0)
                goto nullToken;
        #endif
    }

    // Initialized data
    GXRig_t*      ret         = create_rig();
    size_t        len         = strlen(token);
    size_t        token_count = parse_json(token, len, 0, (void*)0);
    JSONToken_t  *tokens      = calloc(token_count, sizeof(JSONToken_t));

    char         *name        = 0,
                **bones       = 0;

    // Parse the rig object
    parse_json(token, len, token_count, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < token_count; j++)
    {
        // Handle comments
        if      ( strcmp("comment", tokens[j].key) == 0 )
        {
            // Print out comment
            #ifndef NDEBUG
                g_print_log("[G10] [Rig] comment: \"%s\"\n", (char*)tokens[j].value.n_where);
            #endif
            continue;
        }

        // Set name
        else if ( strcmp("name"   , tokens[j].key) == 0 )
        {
            if (tokens[j].type == JSONstring)
                name = tokens[j].value.n_where;
            else
                goto name_type_error;

            continue;
        }

        // Set bones recursively
        else if ( strcmp("bones"  , tokens[j].key) == 0 )
        {
            if (tokens[j].type == JSONobject)
                bones = tokens[j].value.n_where;
            else
                goto bones_type_error;

            continue;
        }
    }

    // Construct rig
    {

        // Set name
        {
            size_t name_len = strlen(tokens[j].value.n_where);

            ret->name = calloc(name_len + 1, sizeof(char));

            // Error checking
            {
                #ifndef NDEBUG
                    if(ret->name == (void *)0)
                        goto no_mem;
                #endif
            }

            strncpy(ret->name, name, name_len);
        }

        // Set bones
        {
            size_t bone_len = strlen(bones);

            ret->bones = load_bone_as_json(bones);

            ret->name[bone_len] = 0;
        }
    }

    free(tokens);

    return ret;

    // Error handling
    {
    nullToken:
        #ifndef NDEBUG
            g_print_error("[G10] [Rig] Null pointer provided in function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }
}

GXBone_t *load_bone       ( const char *path )
{
    // Argument Check
    {
        if (path == 0)
            return (void*)0;
    }

    // Uninitialized data
    size_t       i;
    char*        data;
    int          token_count;
    JSONToken_t* tokens;

    // Initialized data
    GXBone_t* ret = create_bone();
    size_t    l   = 0;
    FILE*     f   = fopen(path, "rb");

    // Load up the file
    i    = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    return load_bone_as_json(data);
}

GXBone_t *load_bone_as_json ( char       *token)
{
    // Argument check
    {
        #ifndef NDEBUG
            if(token == 0)
                goto nullToken;
        #endif
    }

    // Initialized data
    GXBone_t*    ret        = create_bone();
    size_t       len        = strlen(token);
    size_t       token_count = parse_json(token, len, 0, (void*)0);
    JSONToken_t* tokens     = calloc(token_count, sizeof(JSONToken_t));

    // Parse the rig object
    parse_json(token, len, token_count, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < token_count; j++)
    {
        // Set flags
        if (strcmp("name", tokens[j].key) == 0)
        {
            // Initialized data
            char*  name    = tokens[j].value.n_where;
            size_t nameLen = strlen(tokens[j].value.n_where);

            // Allocate for and copy the name
            ret->name = calloc(nameLen+1, sizeof(char));
            strncpy(ret->name, name, nameLen);
            continue;
        }

        // Set head
        else if (strcmp("head", tokens[j].key) == 0)
        {
            ret->head->x = (float) atof(tokens[j].value.a_where[0]),
            ret->head->y = (float) atof(tokens[j].value.a_where[1]),
            ret->head->z = (float) atof(tokens[j].value.a_where[2]),
            ret->head->w = 0.f;
            continue;
        }

        // Set tail
        else if (strcmp("tail", tokens[j].key) == 0)
        {
            ret->tail->x = (float) atof(tokens[j].value.a_where[0]),
            ret->tail->y = (float) atof(tokens[j].value.a_where[1]),
            ret->tail->z = (float) atof(tokens[j].value.a_where[2]),
            ret->tail->w = 0.f;
            continue;
        }

        // Set index
        else if (strcmp("index", tokens[j].key) == 0)
        {
            ret->index = (float) atof(tokens[j].value.n_where);

            continue;
        }

        // Load Children
        else if (strcmp("children", tokens[j].key) == 0)
        {
            // Initialized data
            size_t k = 0,
                   l = 0;

            // Create child bones
            for (l = 0; tokens[j].value.a_where[l]; l++);

            ret->children = calloc(l + 1, sizeof(GXBone_t));

            for (k; k < l; k++)
            {
                ret->children[k]      = *(ret, (*(char*)tokens[j].value.a_where[k] == '{') ? load_bone_as_json(tokens[j].value.a_where[k]) : load_bone(tokens[j].value.a_where[k]));
                ret->children[k].next = &ret->children[k + 1];
                if (AVXSameVec(ret->children[k].head, ret->tail))
                    ret->children[k].connected = true;

            }
            ret->children[k-1].next = (void*)0;
            continue;
        }
    }

    ret->transformation   = calloc(1, sizeof(mat4));

    // Error checking
    {
        #ifndef NDEBUG
            if(ret->transformation == (void *)0)
                goto no_mem;
        #endif
    }

    *ret->transformation  =  identity_mat4();

    free(tokens);

    return ret;

    // Error handling
    {
    nullToken:
        return 0;
        // TODO
    }
}


GXBone_t *find_bone         ( GXBone_t   *bone, char *name, size_t searchDepth)
{
    // Argument check
    {
        #ifndef NDEBUG
            if(bone == (void *)0)
                goto no_bone;
            if (name == (void *)0)
                goto no_name;
        #endif
    }

    // Create a pointer to the head of the list
    GXBone_t* i = bone;

    // Walk through all the bones 
    while (i)
    {
        // Check the first bone
        if (strcmp(i->name, name) == 0)
            return i;

        // If the bone has any children, search through them too
        if (i->children != 0)
            find_bone(i->children, name, searchDepth - 1);

        // Or continue along
        else 
            i = i->next;
    }

    goto noMatch;

    // Error handling
    {
        // Argument errors
        {
            no_bone:
            #ifndef NDEBUG
                g_print_error("[G10] [Rig] Null pointer provided for \"bone\" in call to function \"%s\".\n", __FUNCSIG__);
            #endif
            return 0;

            no_name:
            #ifndef NDEBUG
                g_print_error("[G10] [Rig] Null pointer provided for \"name\" in call to function \"%s\".\n", __FUNCSIG__);
            #endif
            return 0;

        }

        noMatch:
            #ifndef NDEBUG
                g_print_error("[G10] [Rig] There is no bone in \"%s\" named \"%s\".", bone->name, name);
            #endif
            return 0;
    }
}

int       print_rig           ( GXBone_t   *bone )
{
    // Initialized data
    static int indent = -PRINT_INDENT;
    GXBone_t* lBone   = bone;

    // Add to indent
    indent += PRINT_INDENT;

    // Iterate through all bones
    while(lBone)
    {
        // If the bone has a name
        if (lBone->name)
            printf("[G10] [Rig] %d Connected: %c Bone: ", lBone->index, (lBone->connected) ? 'T' : 'F');

        for (size_t i = 0; i < indent; i++)
            putchar(' ');
        (lBone->name) ? printf(" %s\n", lBone->name) : printf("");
        if (lBone->children)
            print_rig(lBone->children);
        lBone = lBone->next;
    }
    indent -= PRINT_INDENT;
    return 0;
}

GXBone_t *remove_bone         ( GXRig_t    *rig,  GXBone_t *bone)
{
    return bone;
}

int       destroy_rig         ( GXRig_t    *rig)
{
    return 0;
}

int       destroy_bone        ( GXBone_t   *bone)
{
	return 0;
}
