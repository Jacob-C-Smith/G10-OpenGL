#include <G10/GXMaterial.h>

GXMaterial_t* createMaterial ( )
{
    // Initialized data
    GXMaterial_t* ret = malloc(sizeof(GXMaterial_t));

    // Check allocated memory
    if (ret == NULL)
        return ret;

    // Zero set the textures
    ret->albedo    = (void*)0;
    ret->normal    = (void*)0;
    ret->roughness = (void*)0;
    ret->metallic  = (void*)0;
    ret->AO        = (void*)0;

    return ret;
}

GXMaterial_t* loadMaterial ( const char path[] )
{
    // Uninitialized data
    int          i;
    u8*          data;
    size_t       len,
                 rootTokenCount;

    // Initialized data
    GXMaterial_t* ret          = malloc(sizeof(GXMaterial_t));
    FILE*         f            = fopen(path, "rb");  

    
    // Check allocated memory
    if (ret == 0)
        return ret;

    {
        // Check if file is valid
        if (f == NULL)
        {
            gPrintError("[G10] [Material] Failed to load file %s\n", path);
            return (void*)0;
        }

        // Find file size and prep for read
        fseek(f, 0, SEEK_END);
        i = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Allocate data and read file into memory
        data = malloc(i);

        // Check if data is valid
        if (data == 0)
            return (void*)0;

        // Read to data
        fread(data, 1, i, f);

        // We no longer need the file
        fclose(f);
    }

    ret = loadMaterialFromJSON(data);

    free(data);

    // Return the material
    return ret;
}

GXMaterial_t* loadMaterialFromJSON(char* token)
{
    // Uninitialized data
    size_t        len,
                  rootTokenCount;
    JSONValue_t  *tokens;

    // Initialized data
    GXMaterial_t* ret = createMaterial();

    // Parse JSON Values
    {
        len = strlen(token), rootTokenCount = GXParseJSON(token, len, 0, 0);
        tokens = calloc(rootTokenCount, sizeof(JSONValue_t));
        GXParseJSON(token, len, rootTokenCount, tokens);
    }

    // TODO: Write code to load many albedos     

    // Find and load the textures
    for (size_t k = 0; k < rootTokenCount; k++)
    {
        if (strncmp("name", tokens[k].name, 4) == 0)
        {
            size_t nameLen = strlen(tokens[k].content.nWhere);
            ret->name = calloc(nameLen + 1, sizeof(u8));
            strncpy(ret->name, tokens[k].content.nWhere, nameLen);
            continue;
        }
        if (strncmp("albedo", tokens[k].name, 6) == 0)
        {
            ret->albedo = loadTexture(tokens[k].content.nWhere);
            continue;
        }
        else if (strncmp("normal", tokens[k].name, 6) == 0)
        {
            ret->normal = loadTexture(tokens[k].content.nWhere);
            continue;
        }
        else if (strncmp("metal", tokens[k].name, 5) == 0)
        {
            ret->metallic = loadTexture(tokens[k].content.nWhere);
            continue;
        }
        else if (strncmp("rough", tokens[k].name, 5) == 0)
        {
            ret->roughness = loadTexture(tokens[k].content.nWhere);
            continue;
        }
        else if (strncmp("AO", tokens[k].name, 2) == 0)
        {
            ret->AO = loadTexture(tokens[k].content.nWhere);
            continue;
        }
        else if (strncmp("height", tokens[k].name, 6) == 0)
        {
            ret->height = loadTexture(tokens[k].content.nWhere);
            continue;
        }
    }
    
    // Free root contents
    free(tokens);

    return ret;
}

int assignMaterial ( GXMaterial_t* material, GXShader_t* shader )
{
    //Bind the texture units to the textureIDs
    size_t albedoIndex = loadTextureToTextureUnit(material->albedo);
    size_t normalIndex = loadTextureToTextureUnit(material->normal);
    size_t roughIndex  = loadTextureToTextureUnit(material->roughness);
    size_t metalIndex  = loadTextureToTextureUnit(material->metallic);
    size_t aoIndex     = loadTextureToTextureUnit(material->AO);

    // Set the texture uniforms from the shader requested data
    setShaderInt(shader, (const char*)findValue(shader->requestedData, shader->requestedDataCount, GXSP_Albedo), albedoIndex);
    setShaderInt(shader, (const char*)findValue(shader->requestedData, shader->requestedDataCount, GXSP_Normal), normalIndex);
    setShaderInt(shader, (const char*)findValue(shader->requestedData, shader->requestedDataCount, GXSP_Rough), roughIndex);
    setShaderInt(shader, (const char*)findValue(shader->requestedData, shader->requestedDataCount, GXSP_Metal), metalIndex);
    setShaderInt(shader, (const char*)findValue(shader->requestedData, shader->requestedDataCount, GXSP_AO), aoIndex);

    return 0;
}

int unloadMaterial ( GXMaterial_t* material )
{
    // Unload all of the textures
    unloadTexture(material->albedo);
    unloadTexture(material->normal);
    unloadTexture(material->metallic);
    unloadTexture(material->roughness);
    unloadTexture(material->AO);

    // Free the material
    free(material);

    return 0;
}
