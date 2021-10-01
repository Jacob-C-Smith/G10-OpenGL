#include <G10/GXMesh.h>

#include <G10/GXPLY.h>


GXPart_t* createPart( )
{
    // Initialized data
    GXPart_t* ret = calloc(1,sizeof(GXPart_t));

    // Check allocated memory
    if (ret == 0)
        return ret;

    // Set vertex arrays, vertex buffers, and element buffers to 0
    ret->vertexArray      = 0;
    ret->vertexBuffer     = 0;
    ret->elementBuffer    = 0;
    ret->elementsInBuffer = 0;
    ret->next             = 0;

    return ret;
}

GXPart_t* loadPart ( const char path[] )
{
    // Uninitialized data
    GXPart_t    *ret;
    size_t       i;
    char        *data;
    int          tokenCount;
    JSONValue_t* tokens;

    // Initialized data
    size_t    l   = 0;
    FILE*     f   = fopen(path, "rb");

    // Load up the file
    i    = gLoadFile(path, 0);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data);

    ret = loadPartAsJSON(data);

    free(data);

    return ret;
}

GXPart_t* loadPartAsJSON ( char* token )
{
    // TODO: Create a mesh JSON schematic
    // TODO: Load as JSON

    // Argument check
    {
        #ifndef NDEBUG
            if (token == 0)
                goto noToken;
        #endif
    }

    // Initialized data
    GXPart_t*    ret        = createPart();
    size_t       len        = strlen(token);
    size_t       tokenCount = GXParseJSON(token, len, 0, (void*)0);
    JSONValue_t* tokens     = calloc(tokenCount, sizeof(JSONValue_t));

    // Parse the camera object
    GXParseJSON(token, len, tokenCount, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < tokenCount; j++)
    {
        // Handle comments
        if (strcmp("name", tokens[j].name) == 0)
        {
            // Initialized data
            char*  path    = tokens[j].content.nWhere;
            size_t pathLen = strlen(path);

            // Copy the string
            ret->name = calloc(pathLen+1, sizeof(char));
            strncpy(ret->name, path, pathLen);

            continue;
        }

        // Process comment
        else if (strcmp("comment", tokens[j].name) == 0)
        {

        }

        // Load from a path
        else if (strncmp("path", tokens[j].name,4) == 0)
        {
            loadPLY(tokens[j].content.nWhere,ret);
        }

        // Set material
        else if (strncmp("material", tokens[j].name,8) == 0)
        {
            ret->material = (*(char*)tokens[j].content.nWhere=='{') ? loadMaterialAsJSON(tokens[j].content.nWhere) : loadMaterial(tokens[j].content.nWhere); 
        }
    }

    free(tokens);

    //ret->next = 0;

    return ret;

    // Error handling
    {
        noToken:
        
        
            return 0;
            // TODO
    }
    
    return ret;
}

GXPart_t* getPart ( GXPart_t* head, const char name[] )
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

    // Iterate through list until we hit the part we want, or zero
    while (i)
    {
        if (strcmp(name, i->name) == 0)
            return i; // If able to locate the part in question, return a pointer
        i = i->next;
    }

    // Unable to locate part
    goto noMatch;

    // Error handling
    {
        // There are no parts
        noParts:
        #ifndef NDEBUG
            gPrintError("[G10] [Mesh] There are no parts.\n");
        #endif
        return 0;

        // There is no matching part
        noMatch:
        #ifndef NDEBUG
            gPrintError("[G10] [Mesh] There is no part named \"%s\".\n", name);
        #endif
        return 0;
        
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            gPrintError("[G10] [Mesh] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int appendPart ( GXPart_t* head, GXPart_t* part )
{
    // Argument checking
    {
        #ifndef NDEBUG
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
        if (strcmp(i->name, part->name) == 0)
            goto duplicateName;
        i = i->next;
    }

    // Assign next as entity
    i->next = part;

    return 0;

    // Error handling
    {
        // Two parts with the same name cannot exist in the same mesh
        duplicateName:
        #ifndef NDEBUG
            gPrintError("[G10] [Mesh] Part \"%s\" can not be appended because a part with that name already exists\n", part->name);
        #endif
        return 0;

        // The mesh argument was null
        nullMesh:
        #ifndef NDEBUG
            gPrintError("[G10] [Mesh] Null pointer provided for \"head\" in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;

        // The part argument was null
        nullPart:
        #ifndef NDEBUG
            gPrintError("[G10] [Mesh] Null pointer provided for \"part\" in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int drawPart ( GXPart_t* part )
{
    // Argument check
    {
        if (part == (void*)0)
            goto noPart;
    }

    // Draw the part
    glBindVertexArray(part->vertexArray);
    glDrawElements(GL_TRIANGLES, part->elementsInBuffer, GL_UNSIGNED_INT, 0);

    return 0;

    // Error handling
    {
        // Null pointer for part
        noPart:
        #ifndef NDEBUG
            gPrintError("[G10] [Mesh] Null pointer provided for parameter \"part\" in function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }
}

GXPart_t *removePart( GXPart_t* head, const char name[] )
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
        destroyPart(i);
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
            gPrintError("[G10] [Mesh] There is no part  named \"%s\".\n", name);
        #endif
        return 0;

        // No head was supplied
        noHead:
        #ifndef NDEBUG
            gPrintError("[G10] [Mesh] Null pointer provided for parameter \"head\" in function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;

        // No name was supplied
        noName:
        #ifndef NDEBUG
            gPrintError("[G10] [Mesh] Null pointer provided for parameter \"name\" in function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }
}

int destroyPart( GXPart_t* part )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(part == 0)
                goto nullPart;
        #endif
    }

    // Free the OpenGL buffers
    glDeleteVertexArrays(1, &part->vertexArray);
    glDeleteBuffers(1, &part->vertexBuffer);
    glDeleteBuffers(1, &part->elementBuffer);

    // Zero set OpenGL buffers
    part->vertexArray      = 0;
    part->vertexBuffer     = 0;
    part->elementBuffer    = 0;
    part->elementsInBuffer = 0;
    destroyTransform(part->transform);

    // Free the part
    free(part->name);

    return 0;

    // Error handling
    {

        // Null parameter for part
        nullPart:
            #ifndef NDEBUG 
                gPrintError("[G10] [Mesh] Null pointer provided for part in function \"%s\"\n", __FUNCSIG__);
            #endif
        return 0;

    }
}

