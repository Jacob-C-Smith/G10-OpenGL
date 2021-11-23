#include <G10/GXEntity.h>

GXEntity_t *createEntity     ( )
{
    // Allocate space
    GXEntity_t* ret = calloc(1,sizeof(GXEntity_t)); 
    
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
            gPrintError("[G10] [Entity] Out of memory.\n");
        #endif
        return 0;
    }
}

GXEntity_t *loadEntity       ( const char path[] )
{
    // Uninitialized data
    u8         *data;
    GXEntity_t *ret;

    // Initialized data
    size_t      i   = 0;

    #ifndef NDEBUG
        gPrintLog("[G10] [Entity] Loading \"%s\".\n", (char*)path);
    #endif	

    // Load up the file
    i    = gLoadFile(path, 0, false);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data, false);

    ret  = loadEntityAsJSON(data);

    // Finish up
    free(data);

    return ret;

    // Error handling
    {
        invalidFile:
            #ifndef NDEBUG
                gPrintError("[G10] [Entity] Failed to load file %s\n", path);
            #endif
        return 0;
    }
}

GXEntity_t *loadEntityAsJSON ( char* token )
{
    // Initialized data
    GXEntity_t  *ret        = createEntity();
    size_t       len        = strlen(token),
                 tokenCount = GXParseJSON(token, len, 0, (void*)0);
    JSONValue_t *tokens     = calloc(tokenCount, sizeof(JSONValue_t));

    // Parse the entity object
    GXParseJSON(token, len, tokenCount, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < tokenCount; j++)
    {
        // Handle comments
        if (strcmp("comment", tokens[j].name) == 0)
        {
            #ifndef NDEBUG
                // Print out comment
                gPrintLog("[G10] [Entity] Comment: \"%s\"\n", (char*)tokens[j].content.nWhere);
            #endif
            continue;
        }

        // Set name
        else if (strcmp("name", tokens[j].name) == 0)
        {
            // Inialized data
            size_t len = strlen(tokens[j].content.nWhere);

            ret->name = calloc(len + 1,sizeof(char));

            // Copy name
            strncpy(ret->name, tokens[j].content.nWhere, len);
            ret->name[len] = 0;
            continue;
        }

        // Load and process a mesh
        else if (strcmp("parts", tokens[j].name) == 0)
        {
            ret->parts = (*(char*)tokens[j].content.aWhere[0] == '{') ? loadPartAsJSON((const char*)tokens[j].content.aWhere[0]) : loadPart(tokens[j].content.aWhere[0]);
            
            for (size_t k = 1; tokens[j].content.aWhere[k]; k++)
                appendPart(ret->parts,(*(char*)tokens[j].content.aWhere[k] == '{') ? loadPartAsJSON((const char*)tokens[j].content.aWhere[k]) : loadPart(tokens[j].content.aWhere[k]));
          
            continue;
        }

        // Process a shader
        else if (strcmp("shader", tokens[j].name) == 0)
        {
            ret->shader = (*(char*)tokens[j].content.nWhere == '{') ? loadShaderAsJSON((const char*)tokens[j].content.nWhere) : loadShader(tokens[j].content.nWhere);

            continue;
        }

        // Process materials
        else if (strcmp("materials", tokens[j].name) == 0)
        {
            ret->materials = (*(char*)tokens[j].content.aWhere[0] == '{') ? loadMaterialAsJSON((const char*)tokens[j].content.aWhere[0]) : loadMaterial(tokens[j].content.aWhere[0]);

            for (size_t k = 1; tokens[j].content.aWhere[k]; k++)
                appendMaterial(ret->materials, (*(char*)tokens[j].content.aWhere[k] == '{') ? loadMaterialAsJSON((const char*)tokens[j].content.aWhere[k]) : loadMaterial(tokens[j].content.aWhere[k]));

        }

        // Process a transform
        else if (strcmp("transform", tokens[j].name) == 0)
        {
            // TODO: Test inline transform
            ret->transform = (*(char*)tokens[j].content.nWhere == '{') ? loadTransformAsJSON(tokens[j].content.nWhere) : loadTransform(tokens[j].content.nWhere);


            continue;
        }

        // Process a rigidbody
        else if (strcmp("rigid body", tokens[j].name) == 0)
        {
            ret->rigidbody = (*(char*)tokens[j].content.nWhere == '{') ? loadRigidbodyAsJSON((char*)tokens[j].content.nWhere) : loadRigidbody((const char*)tokens[j].content.nWhere);

            continue;
        }

        // Process a collider
        else if (strcmp("collider", tokens[j].name) == 0)
        {
            // TODO: Write
            ret->collider = (*(char*)tokens[j].content.nWhere == '{') ? loadColliderAsJSON((char*)tokens[j].content.nWhere) : loadCollider((const char*)tokens[j].content.nWhere);
            continue;
        }
    }

    free(tokens);

    return ret;
}

int         drawEntity       ( GXEntity_t* entity)
{
    // Draw the parts
    GXPart_t *i = entity->parts;

    // Draw the entire entity
    while (i)
    {
        entity->transform->modelMatrix = mat4xmat4(rotationMatrixFromQuaternion(entity->transform->rotation), translationScaleMat(entity->transform->location, entity->transform->scale));
        setShaderTransform(entity->shader, entity->transform);
        setShaderMaterial(entity->shader, getMaterial(entity->materials, i->material));

        drawPart(i);
        i = i->next;
    }

    return 0;
}

int         destroyEntity    ( GXEntity_t* entity )
{
    // TODO: Use destructors functions

    // Check to see if items are set before we unload them
    if (entity->name != (void*)0)
    {
        free(entity->name);
        entity->name = (void*)0;
    }
    if (entity->parts != (void*)0)
        // TODO: Destroy all parts

    if (entity->shader != (void*)0)
        entity->shader = (void*)0;

    if (entity->transform != (void*)0)
        entity->transform = (void*)0;

    if (entity->rigidbody != (void*)0)
        entity->rigidbody = (void*)0;

    if (entity->collider != (void*)0)
        entity->collider = (void*)0;

    entity->next = (void*)0;

    // Free the entity
    free(entity);

    return 0;
}
