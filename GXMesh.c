#include <G10/GXMesh.h>

#include <G10/GXPLY.h>

GXMesh_t* createMesh ( )
{
    // Initialized data
    GXMesh_t* ret = malloc(sizeof(GXMesh_t));

    // Check allocated memory
    if (ret == 0)
        return ret;

    // Zero set everything
    ret->name       = (void*)0;
    ret->parts      = (void*)0;

    return ret;
}

GXPart_t* createPart( )
{
    // Initialized data
    GXPart_t* ret = malloc(sizeof(GXPart_t));

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

GXMesh_t* loadMesh(const char path[])
{
    // Argument Check
    {
        if (path == 0)
            return (void*)0;
    }

    // Uninitialized data
    int          i;
    char        *data;
    int          tokenCount;
    JSONValue_t *tokens;

    // Initialized data
    GXMesh_t    *ret = createMesh();
    size_t       l   = 0;
    FILE        *f   = fopen(path, "rb");

    // Load the file
    {
        // Check if file is valid
        if (f == NULL)
        {
            printf("[G10] [Mesh] Failed to load file %s\n", path);
            return (void*)0;
        }

        // Find file size and prep for read
        fseek(f, 0, SEEK_END);
        i = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Allocate data and read file into memory
        data = malloc(i + 1);
        if (data == 0)
            return (void*)0;
        fread(data, 1, i, f);

        // We no longer need the file
        fclose(f);

        // For reasons beyond me, the null terminator isn't included.
        data[i] = '\0';
    }

    return loadMeshAsJSON(data);
}

GXMesh_t* loadMeshAsJSON(char* token)
{
    // Argument check
    {
        #ifndef NDEBUG
            if(token == 0)
                goto nullToken;
        #endif
    }

    // Initialized data
    GXMesh_t*    ret        = createMesh();
    size_t       len        = strlen(token);
    size_t       tokenCount = GXParseJSON(token, len, 0, (void*)0);
    JSONValue_t* tokens     = calloc(tokenCount, sizeof(JSONValue_t));

    // Parse the camera object
    GXParseJSON(token, len, tokenCount, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < tokenCount; j++)
    {
        // Handle comments
        if (strcmp("comment", tokens[j].name) == 0)
        {
            // Print out comment
            #ifndef NDEBUG
            printf("[G10] [Mesh] Comment: \"%s\"\n", (char*)tokens[j].content.nWhere);
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
        else if (strcmp("parts", tokens[j].name) == 0)
        {
            // Inialized data
            size_t len = strlen(tokens[j].content.nWhere);

            for (size_t k = 0; tokens[j].content.aWhere[k]; k++)
                appendPart(ret, (*(char*)tokens[j].content.aWhere[k] == '{') ? loadPartAsJSON(tokens[j].content.aWhere[k]) : loadPart(tokens[j].content.aWhere[k]));
            
            ret->name[len] = 0;
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

GXPart_t* loadPart ( const char path[] )
{
    // Uninitialized data
    int          i;
    char*        data;
    int          tokenCount;
    JSONValue_t* tokens;

    // Initialized data
    size_t    l   = 0;
    FILE*     f   = fopen(path, "rb");


    // Load the file
    {
        // Check if file is valid
        if (f == NULL)
        {
            printf("[G10] [Mesh] Failed to load file %s\n", path);
            return (void*)0;
        }

        // Find file size and prep for read
        fseek(f, 0, SEEK_END);
        i = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Allocate data and read file into memory
        data = malloc(i + 1);
        if (data == 0)
            return (void*)0;
        fread(data, 1, i, f);

        // We no longer need the file
        fclose(f);

        // For reasons beyond me, the null terminator isn't included.
        data[i] = '\0';
    }

    return loadPartAsJSON(data);
}

GXPart_t* loadPartAsJSON ( char* token )
{
    // TODO: Create a mesh JSON schematic
    // TODO: Load as JSON

    // Argument check
    {
        #ifndef NDEBUG
            if (token == 0)
                goto nullToken;
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
        else if (strcmp("path", tokens[j].name) == 0)
        {
            loadPLY(tokens[j].content.nWhere,ret);
        }

        // Set name
        else if (strcmp("material", tokens[j].name) == 0)
        {
            ret->material = loadMaterial(tokens[j].content.nWhere);
        }
    }

    free(tokens);

    //ret->next = 0;

    return ret;

    // Error handling
    {
    nullToken:
        return 0;
        // TODO
    }
    
    return ret;
}

GXPart_t* getPart ( GXMesh_t* mesh, const char name[] )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if (mesh == 0)
                goto nullMesh;
            if (name == 0)
                goto nullName;
        #endif  
    }

    // Create a pointer to the head of the list
    GXPart_t* i = mesh->parts;

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
            printf("[G10] [Mesh] There are no parts in \"%s\".\n", mesh->name);
        #endif
        return 0;

        // There is no matching part
        noMatch:
        #ifndef NDEBUG
            printf("[G10] [Mesh] There is no part in \"%s\" named \"%s\".", mesh->name, name);
        #endif
        return 0;
    
        // The mesh parameter was null
        nullMesh:
        #ifndef NDEBUG
            printf("[G10] [Mesh] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            printf("[G10] [Mesh] Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int appendPart ( GXMesh_t* mesh, GXPart_t* part )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if(mesh == 0)
                goto nullMesh;
            if(part == 0)
                goto nullPart;
        #endif
    }

    // Set the pointer to the head of the linked list
    GXPart_t* i = mesh->parts;

    // Check if the head is null. If so, set the head to the mesh
    if (i == 0)
    {
        mesh->parts = part;
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
            printf("[G10] [Mesh] Part \"%s\" can not be appended to \"%s\" because a part with that name already exists\n", part->name, mesh->name);
        #endif
        return 0;

        // The mesh argument was null
        nullMesh:
        #ifndef NDEBUG
            printf("[G10] [Mesh] Null pointer provided for mesh in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;

        // The part argument was null
        nullPart:
        #ifndef NDEBUG
            printf("[G10] [Mesh] Null pointer provided for part in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int drawMesh ( GXMesh_t* mesh, GXShader_t* shader ) 
{
    // Initialized data
    GXPart_t* i = mesh->parts;

    // Draw the entire mesh
    while (i)
    {
        if (i->material)
            assignMaterial(i->material, shader);
        drawPart(i);
        i = i->next;
    }

    return 0;
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
            printf("[G10] [Mesh] Null pointer provided for parameter \"part\" in function \"%s\"\n",__FUNCSIG__);
        #endif
    }
}

int removePart( GXMesh_t* mesh, const char name[] )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(mesh == (void*)0)
                goto noMesh;
            if(name == (void*)0)
                goto noName;
        #endif
    }

    // Create a pointer to the head of the list
    GXPart_t* i = mesh->parts;

    // Quick sanity check
    if (i == 0)
        goto noParts;

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
            destroyPart(i->next);

            // Verbose logging
            #ifndef NDEBUG
                printf("[G10] [Mesh] Destroyed part \"%s\" from mesh \"%s\"\n", name,mesh->name);
            #endif		

            // Stitch up the linked list 
            i->next = j;
            return 0;
        }
        i = i->next;
    }
    goto noMatch;

    // Error handling
    {
        // Error handling if the mesh is empty.
        noParts:
        #ifndef NDEBUG
            printf("[G10] [Mesh] There are no parts in \"%s\"\n", mesh->name);
        #endif
        return 0;

        // Error hadling if we don't find the mesh.
        noMatch:
        #ifndef NDEBUG
            printf("[G10] [Mesh] There is no part in \"%s\" named \"%s\".\n", mesh->name, name);
        #endif
        return 0;

        // No mesh was supplied
        noMesh:
        #ifndef NDEBUG
            printf("[G10] [Mesh] Null pointer provided for parameter \"mesh\" in function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;

        // No name was supplied
        noName:
        #ifndef NDEBUG
            printf("[G10] [Mesh] Null pointer provided for parameter \"name\" in function \"%s\"\n",__FUNCSIG__);
        #endif
        return 0;
    }
}

int destroyMesh ( GXMesh_t* mesh )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(mesh==0)
                goto noMesh;
        #endif
    }

    // Initialized data
    GXPart_t* i = mesh->parts;

    // Free the name
    free(mesh->name);

    // Free all the parts
    while (i)
    {
        destroyPart(i);
        i = i->next;
    }

    // Free the mesh
    free(mesh);

    // Exit
    return 0;

    // Error handling
    {
        noMesh:
            #ifndef NDEBUG
                printf("[G10] [Mesh] Null pointer provided for mesh in function \"%s\"\n", __FUNCSIG__);
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

    // Free the part
    free(part);
    return 0;

    // Error handling
    {

        // Null parameter for part
        nullPart:
            #ifndef NDEBUG 
                printf("[G10] [Mesh] Null pointer provided for part in function \"%s\"\n", __FUNCSIG__);
            #endif
        return 0;

    }
}

