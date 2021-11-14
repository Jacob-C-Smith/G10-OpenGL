#include <G10/GXRig.h>

GXRig_t  *createRig          ( )
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
            gPrintError("[G10] [Camera] Out of memory.\n");
        #endif
        return 0;
    }
}

GXBone_t *createBone         ( )
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
            gPrintError("[G10] [Camera] Out of memory.\n");
        #endif
        return 0;
    }
}

GXRig_t  *loadRig            ( const char *path )
{
    // Argument Check
    {
        if (path == 0)
            return (void*)0;
    }

    // Uninitialized data
    size_t       i;
    char        *data;
    int          tokenCount;
    JSONValue_t *tokens;
    GXRig_t     *ret;

    // Initialized data
    FILE        *f = fopen(path, "rb");

    // Load the file
    i    = gLoadFile(path, 0, false);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data, false);
    
    ret = loadRigAsJSON(data);

    free(data);

    return ret;
}

GXRig_t  *loadRigAsJSON      ( char       *token )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(token == 0)
                goto nullToken;
        #endif
    }

    // Initialized data
    GXRig_t*     ret        = createRig();
    size_t       len        = strlen(token);
    size_t       tokenCount = GXParseJSON(token, len, 0, (void*)0);
    JSONValue_t* tokens     = calloc(tokenCount, sizeof(JSONValue_t));

    // Parse the rig object
    GXParseJSON(token, len, tokenCount, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < tokenCount; j++)
    {
        // Handle comments
        if (strcmp("comment", tokens[j].name) == 0)
        {
            // Print out comment
            #ifndef NDEBUG
                gPrintLog("[G10] [Rig] comment: \"%s\"\n", (char*)tokens[j].content.nWhere);
            #endif
            continue;
        }

        // Set flags
        else if (strcmp("name", tokens[j].name) == 0)
        {
            // Initialized data
            char*  name    = tokens[j].content.nWhere;
            size_t nameLen = strlen(tokens[j].content.nWhere);

            // Allocate for and copy the name
            ret->name = calloc(nameLen+1, sizeof(char));
            strncpy(ret->name, name, nameLen);
            continue;
        }

        // Set name
        else if (strcmp("armiture", tokens[j].name) == 0)
        {
            // Inialized data
            size_t len = strlen(tokens[j].content.nWhere);

            ret->bones = loadArmitureAsJSON(tokens[j].content.nWhere);
            
            ret->name[len] = 0;
            continue;
        }
    }

    free(tokens);

    return ret;

    // Error handling
    {
    nullToken:
        #ifndef NDEBUG
            gPrintError("[G10] [Rig] Null pointer provided in function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }
}

GXBone_t *loadArmiture       ( const char *path )
{
    // Argument Check
    {
        if (path == 0)
            return (void*)0;
    }

    // Uninitialized data
    size_t       i;
    char*        data;
    int          tokenCount;
    JSONValue_t* tokens;

    // Initialized data
    GXBone_t* ret = createBone();
    size_t    l   = 0;
    FILE*     f   = fopen(path, "rb");

    // Load up the file
    i    = gLoadFile(path, 0, false);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data, false);

    return loadRigAsJSON(data);
}

GXBone_t *loadArmitureAsJSON ( char       *token)
{
    // Argument check
    {
        #ifndef NDEBUG
            if(token == 0)
                goto nullToken;
        #endif
    }

    // Initialized data
    GXBone_t*    ret        = createBone();
    size_t       len        = strlen(token);
    size_t       tokenCount = GXParseJSON(token, len, 0, (void*)0);
    JSONValue_t* tokens     = calloc(tokenCount, sizeof(JSONValue_t));

    // Parse the rig object
    GXParseJSON(token, len, tokenCount, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < tokenCount; j++)
    {
        // Set flags
        if (strcmp("name", tokens[j].name) == 0)
        {
            // Initialized data
            char*  name    = tokens[j].content.nWhere;
            size_t nameLen = strlen(tokens[j].content.nWhere);

            // Allocate for and copy the name
            ret->name = calloc(nameLen+1, sizeof(char));
            strncpy(ret->name, name, nameLen);
            continue;
        }

        // Set head
        else if (strcmp("head", tokens[j].name) == 0)
        {
            ret->head->x = (float) atof(tokens[j].content.aWhere[0]),
            ret->head->y = (float) atof(tokens[j].content.aWhere[1]),
            ret->head->z = (float) atof(tokens[j].content.aWhere[2]),
            ret->head->w = 0.f;
            continue;
        }

        // Set tail
        else if (strcmp("tail", tokens[j].name) == 0)
        {
            ret->tail->x = (float) atof(tokens[j].content.aWhere[0]),
            ret->tail->y = (float) atof(tokens[j].content.aWhere[1]),
            ret->tail->z = (float) atof(tokens[j].content.aWhere[2]),
            ret->tail->w = 0.f;
            continue;
        }

        // Load Children
        else if (strcmp("children", tokens[j].name) == 0)
        {
            // Initialized data
            size_t k = 0,
                   l = 0;

            // Create child bones
            for (l = 0; tokens[j].content.aWhere[l]; l++);

            ret->children = calloc(l + 1, sizeof(GXBone_t));

            for (k; k < l; k++)
            {
                ret->children[k]      = *(ret, (*(char*)tokens[j].content.aWhere[k] == '{') ? loadArmitureAsJSON(tokens[j].content.aWhere[k]) : loadArmiture(tokens[j].content.aWhere[k]));
                ret->children[k].next = &ret->children[k + 1];
                if (AVXSameVec(ret->children[k].head, ret->tail))
                    ret->children[k].connected = true;

            }
            ret->children[k-1].next = (void*)0;
            continue;
        }
    }

    free(tokens);

    return ret;

    // Error handling
    {
    nullToken:
        return 0;
        // TODO
    }
}

GXBone_t *findBone           ( GXRig_t    *rig,  char *name)
{
    // TODO: Argument check
    {
        #ifndef NDEBUG
        if (rig == (void*)0)
            goto noRig;
        if (name == (void*)0)
            goto noName;
        #endif
    }

    // Create a pointer to the head of the list
    GXBone_t* i = rig->bones;

    // Sanity check
    if (i == 0)
        goto noBones;

    return searchBone(rig, name, 0);

    // Error handling
    {
        noBones:
            #ifndef NDEBUG
                gPrintError("[G10] [Rig] There are no bones in \"%s\".\n", rig->name);
            #endif
            return 0;

        noMatch:
            #ifndef NDEBUG
                gPrintError("[G10] [Rig] There is no bone in \"%s\" named \"%s\".", rig->name, name);
            #endif
            return 0;
        noRig:
            #ifndef NDEBUG
                gPrintError("[G10] [Rig] No rig provided to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        noName:
            #ifndef NDEBUG
                gPrintError("[G10] [Rig] No name provided to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
    }
}

GXBone_t *searchBone         ( GXBone_t   *bone, char *name, size_t searchDepth)
{
    // Create a pointer to the head of the list
    GXBone_t* i = bone;

    // Sanity check
    if (i == 0)
        goto noBones;

    // Walk through all the bones 
    while (i)
    {
        // Check the first bone
        if (strcmp(i->name, name) == 0)
            return i;

        // If the bone has any children, search through them too
        if (i->children != 0)
            searchBone(i->children, name, searchDepth - 1);

        // Or continue along
        else 
            i = i->next;
    }

    goto noMatch;

    // Error handling
    {
        noBones:
            #ifndef NDEBUG
                gPrintError("[G10] [Rig] There are no bones in \"%s\".\n", bone->name);
            #endif
            return 0;

        noMatch:
            #ifndef NDEBUG
                gPrintError("[G10] [Rig] There is no bone in \"%s\" named \"%s\".", bone->name, name);
            #endif
            return 0;
    }
}

int       printRig           ( GXBone_t   *bone )
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
            printf("[G10] [Rig] Connected: %c Bone: ",(lBone->connected) ? 'T' : 'F');

        for (size_t i = 0; i < indent; i++)
            putchar(' ');
        (lBone->name) ? printf(" %s\n", lBone->name) : printf("");
        if (lBone->children)
            printRig(lBone->children);
        lBone = lBone->next;
    }
    indent -= PRINT_INDENT;
}

GXBone_t *removeBone         ( GXRig_t    *rig,  GXBone_t *bone)
{
    return bone;
}

int       destroyRig         ( GXRig_t    *rig)
{
    return 0;
}

int       destroyBone        ( GXBone_t   *bone)
{
	return 0;
}
