#include <G10/GXEntity.h>

GXEntity_t* createEntity ( )
{
    // Allocate space
    GXEntity_t* ret = calloc(1,sizeof(GXEntity_t)); 

    // Check if valid
    if (ret == 0)
        return ret;

    // Set flags
    ret->flags     = 0;

    // Initialize to nullptr
    ret->name      = (void*)0;
    ret->mesh      = (void*)0;
    ret->shader    = (void*)0;
    ret->transform = (void*)0;
    ret->rigidbody = (void*)0;
    ret->collider  = (void*)0;

    ret->next      = (void*)0;

    return ret;
}

int drawEntity ( GXEntity_t* entity )
{
    // If the drawable flag is not set, we can stop
    //if (!(entity->flags & GXEFDrawable))

    for (size_t j = 0; j < entity->shader->requestedDataCount; j++)
    {
        switch (entity->shader->requestedData[j].key)
        {
            case GXSP_Model:
                entity->transform->modelMatrix = mat4xmat4(rotationMatrixFromQuaternion(makeQuaternionFromEulerAngle(entity->transform->rotation)), translationScaleMat(entity->transform->location, entity->transform->scale));
                setShaderMat4(entity->shader, entity->shader->requestedData[j].value, &entity->transform->modelMatrix);
                break;
            default:
                break;
        }
    }

    // Assign the material
    //assignMaterial(entity->material, entity->shader);

    // Draw the mesh
    drawMesh(entity->mesh,entity->shader);

    return 0;
}

GXEntity_t* loadEntity ( const char path[] )
{
    // Uninitialized data
    u8         *data;
    GXEntity_t *ret;

    // Initialized data
    size_t      i   = 0;
    FILE       *f   = fopen(path, "rb");

    #ifndef NDEBUG
        gPrintLog("[G10] [Entity] Loading \"%s\".\n", (char*)path);
    #endif	

    // Load up the file
    i = gLoadFile(path, 0);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data);

    ret = loadEntityAsJSON(data);

    // Finish up
    free(data);

    return ret;
    invalidFile:
    #ifndef NDEBUG
        gPrintError("[G10] [Entity] Failed to load file %s\n", path);
    #endif
    return 0;
}

GXEntity_t* loadEntityAsJSON ( char* token )
{
    // Initialized data
    GXEntity_t*  ret        = createEntity();
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
            #ifndef NDEBUG
                // Print out comment
                gPrintLog("[G10] [Entity] Comment: \"%s\"\n", (char*)tokens[j].content.nWhere);
            #endif
            continue;
        }

        // Set flags
        else if (strcmp("flags", tokens[j].name) == 0)
        {
            // TODO: If flags are set, then we will use the provided flags. If flags are not set,
            // we will detect what flags can be set automatically
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
        else if (strcmp("mesh", tokens[j].name) == 0)
        {
            // TODO: Test inline mesh
            ret->mesh = (*(char*)tokens[j].content.nWhere == '{') ? loadMeshAsJSON((const char*)tokens[j].content.nWhere) : loadMesh(tokens[j].content.nWhere);
          
            ret->flags |= GXEFMesh;
            continue;
        }

        // Process a shader
        else if (strcmp("shader", tokens[j].name) == 0)
        {
            // TODO: Test inline shader
            ret->shader = (*(char*)tokens[j].content.nWhere == '{') ? loadShaderAsJSON((const char*)tokens[j].content.nWhere) : loadShader(tokens[j].content.nWhere);

            ret->flags |= GXEFShader;
            continue;
        }

        // Process a transform
        else if (strcmp("transform", tokens[j].name) == 0)
        {
            // TODO: Test inline transform
            ret->transform = (*(char*)tokens[j].content.nWhere == '{') ? loadTransformAsJSON(tokens[j].content.nWhere) : loadTransform(tokens[j].content.nWhere);

            // Set the transform flag
            ret->flags |= GXEFTransform;

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
            //ret->collider = (*(char*)tokens[j].content.nWhere == '{') ? loadColliderAsJSON((char*)tokens[j].content.nWhere) : loadCollider((const char*)tokens[j].content.nWhere);
            continue;
        }
    }

    free(tokens);

    ret->next   = 0;
    ret->flags |= GXEFPresent;

    return ret;
}

int destroyEntity ( GXEntity_t* entity )
{
    // TODO: Use destructors functions

    // Unset all flags
    entity->flags = 0;

    // Check to see if items are set before we unload them
    if (entity->name != (void*)0)
    {
        free(entity->name);
        entity->name = (void*)0;
    }
    if (entity->mesh != (void*)0)
        destroyMesh(entity->mesh);

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
