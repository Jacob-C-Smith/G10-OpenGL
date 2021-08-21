#include <G10/GXTransform.h>

GXTransform_t* createTransform ( GXvec3_t location, GXvec3_t rotation, GXvec3_t scale )
{
    // Allocate space
    GXTransform_t* ret = calloc(1,sizeof(GXTransform_t));

    // Check if valid
    if (ret == 0)
        return (void*)0;

    // Set location, rotation, and scale
    ret->location = location;
    ret->rotation = rotation;
    ret->scale    = scale;

    // Create a model matrix from the location, rotation, and scale
    ret->modelMatrix = mat4xmat4(rotationMatrixFromQuaternion(makeQuaternionFromEulerAngle(rotation)), translationScaleMat(location, scale));

    // Debug information
    #ifndef NDEBUG
        gPrintLog("[G10] [Transform] Transform created with location (%f,%f,%f)\n" 
                  "                                         rotation (%f,%f,%f)\n"
                  "                                         scale    (%f,%f,%f)\n"
                  , location.x, location.y, location.z, rotation.x, rotation.y, rotation.z, scale.x, scale.y, scale.z);
    #endif

    return ret;
}

GXTransform_t* loadTransform ( const char path[] )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(path==(void*)0)
                goto noArgument;
        #endif
    }

    // Uninitialized data
    u8*            data;
    GXTransform_t* ret;

    // Initialized data
    size_t         l = 0;
    FILE*          f = fopen(path, "rb");

    // Load the file
    {
        // Check for file
        if (f == NULL)
            goto invalidFile;

        // Find file size and prep for read
        fseek(f, 0, SEEK_END);
        l = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Allocate data and read file into memory
        data = calloc(l + 1,sizeof(char));
        fread(data, 1, l, f);

        // We no longer need the file
        fclose(f);
    }

    ret = loadTransformAsJSON(data);

    // Finish up
    free(data);

    return ret;
    
    // Error handling
    {
        // File not found
        invalidFile:
        #ifndef NDEBUG
            gPrintError("[G10] [Transform] Failed to load file %s\n", path);
        #endif
        return 0;

        // Path was null pointer
        noArgument:
        #ifndef NDEBUG
            gPrintError("[G10] [Transform] No file supplied for path in function %s\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXTransform_t* loadTransformAsJSON ( char* token ) 
{
    // Initialiazed data
    GXTransform_t* ret        = 0;
    size_t         len        = strlen(token),
                   tokenCount = GXParseJSON(token, len, 0, 0);
    JSONValue_t*   tokens     = calloc(tokenCount, sizeof(JSONValue_t));
    GXvec3_t       location   = { 0,0,0 },
                   rotation   = { 0,0,0 },
                   scale      = { 0,0,0 };

    // Parse JSON Values
    GXParseJSON(token, len, tokenCount, tokens);

    // Find and assign location, rotation, and scale vectors
    for ( size_t k = 0; k < tokenCount; k++ )
        if ( strcmp( "location", tokens[k].name ) == 0 )
            location = (GXvec3_t) { ( float ) atof(tokens[k].content.aWhere[0] ), ( float ) atof(tokens[k].content.aWhere[1] ), ( float ) atof(tokens[k].content.aWhere[2] ) };
        else if ( strcmp( "rotation", tokens[k].name ) == 0 )
            rotation = (GXvec3_t) { ( float ) atof(tokens[k].content.aWhere[0] ), ( float ) atof(tokens[k].content.aWhere[1] ), ( float ) atof(tokens[k].content.aWhere[2] ) };
        else if ( strcmp( "scale", tokens[k].name ) == 0 )
            scale    = (GXvec3_t) { ( float ) atof(tokens[k].content.aWhere[0] ), ( float ) atof(tokens[k].content.aWhere[1] ), ( float ) atof(tokens[k].content.aWhere[2] ) };
    
    // Free subcontents
    free(tokens);


    // TODO: Rewrite createTransform 
    // Process transform
    return createTransform(location, rotation, scale);
}



int unloadTransform ( GXTransform_t* transform )
{
    // Zero set everything
    transform->location    = (GXvec3_t){ 0,0,0 };
    transform->rotation    = (GXvec3_t){ 0,0,0 };
    transform->scale       = (GXvec3_t){ 0,0,0 };
    transform->modelMatrix = (GXmat4_t){ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    // Free the transform
    free(transform);

    return 0;
}
