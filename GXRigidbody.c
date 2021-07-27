#include <G10/GXRigidbody.h>

GXRigidbody_t* createRigidbody ( )
{
    // Initialized data
    GXRigidbody_t* ret = malloc(sizeof(GXRigidbody_t));

    if (ret == (void*)0)
        return ret;

    ret->mass                = 0.f;
    ret->radius              = 0.f;

    ret->acceleration        = (GXvec3_t){ 0.f, 0.f, 0.f, 0.f };
    ret->velocity            = (GXvec3_t){ 0.f, 0.f, 0.f, 0.f };
    
    ret->angularAcceleration = (GXvec3_t){ 0.f, 0.f, 0.f, 0.f };
    ret->angularVelocity     = (GXvec3_t){ 0.f, 0.f, 0.f, 0.f };

    ret->forces              = calloc(MAXFORCES, sizeof(GXvec3_t));

    ret->forcesCount         =  1;
    
    return ret;
}

GXRigidbody_t* loadRigidbody(const char path[])
{
    // Initialized data
    GXRigidbody_t* ret      = 0;
    FILE*          f        = fopen(path, "rb");
    size_t         i        = 0;
    char*          data     = 0;

    // Load the file
    {
        // Check if file is valid
        if (f == NULL)
            goto invalidFile;

        // Find file size and prep for read
        fseek(f, 0, SEEK_END);
        i = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Allocate data and read file into memory
        data = malloc(i);
        if (data == 0)
            return (void*)0;
        fread(data, 1, i, f);
    
        // We no longer need the file
        fclose(f);
    }

    // Load the camera from data
    ret = loadRigidbodyAsJSON(data);

    // Free the camera
    free(data);

    return ret;
    invalidFile:
    #ifndef NDEBUG
        gPrintError("[G10] [Rigidbody] Failed to load file %s\n", path);
    #endif
    return 0;
}

GXRigidbody_t* loadRigidbodyAsJSON(char* token)
{
    // Initialized data
    GXRigidbody_t*  ret        = createRigidbody();
    size_t          len        = strlen(token);
    size_t          tokenCount = GXParseJSON(token, len, 0, (void*)0);
    JSONValue_t*    tokens     = calloc(tokenCount, sizeof(JSONValue_t));

    // Parse the rigid body  object
    GXParseJSON(token, len, tokenCount, tokens);

    // Copy relevent data for a rigid body object
    for (size_t l = 0; l < tokenCount; l++)
    {

        // Parse out where the camera is
        if (strcmp("mass", tokens[l].name) == 0)
        {
            ret->mass = (float) atof(tokens[l].content.nWhere);
            continue;
        }

        // Parse out target
        else if (strcmp("radius", tokens[l].name) == 0)
        {
            ret->radius = (float) atof(tokens[l].content.nWhere);
            continue;
        }

        // Parse out up
        else if (strcmp("acceleration", tokens[l].name) == 0)
        {
            ret->acceleration = (GXvec3_t){ (float)atof(tokens[l].content.aWhere[0]), (float)atof(tokens[l].content.aWhere[1]), (float)atof(tokens[l].content.aWhere[2]), 0.f };
            continue;
        }

        // Parse out FOV
        else if (strcmp("velocity", tokens[l].name) == 0)
        {
            ret->velocity = (GXvec3_t){ (float)atof(tokens[l].content.aWhere[0]), (float)atof(tokens[l].content.aWhere[1]), (float)atof(tokens[l].content.aWhere[2]), 0.f };
            continue;
        }

        // Parse out near clipping plane
        else if (strcmp("angular acceleration", tokens[l].name) == 0)
        {
            ret->angularAcceleration = (GXvec3_t){ (float)atof(tokens[l].content.aWhere[0]), (float)atof(tokens[l].content.aWhere[1]), (float)atof(tokens[l].content.aWhere[2]), 0.f };
            continue;
        }

        // Parse out far clipping plane
        else if (strcmp("angular velocity", tokens[l].name) == 0)
        {
            ret->angularVelocity = (GXvec3_t){ (float)atof(tokens[l].content.aWhere[0]), (float)atof(tokens[l].content.aWhere[1]), (float)atof(tokens[l].content.aWhere[2]), 0.f };
            continue;
        }
    }

    free(tokens);

    return ret;
}

int updatePositionAndVelocity ( GXRigidbody_t* rigidbody, GXTransform_t* transform, u32 deltaTime )
{
    // Initialized data
    GXvec3_t f = applyForce(rigidbody);
    GXvec3_t a = vec3xf(f, 1 / rigidbody->mass);

    rigidbody->velocity = addVec3(rigidbody->velocity, vec3xf(a, deltaTime / 1000.f));
    transform->location = addVec3(transform->location, vec3xf(rigidbody->velocity, deltaTime / 1000.f));

    return 0;
}

int destroyRigidBody ( GXRigidbody_t* rigidbody )
{
    rigidbody->mass         = 0;
    rigidbody->acceleration = (GXvec3_t){ 0.f,0.f,0.f };

    free(rigidbody);

    return 0;
}
