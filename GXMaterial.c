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
    JSONValue_t* rootContents;

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
            printf("Failed to load file %s\n", path);
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

    // Parse JSON Values
    len          = strlen(data), rootTokenCount = GXParseJSON(data, len, 0, 0);
    rootContents = calloc(rootTokenCount, sizeof(JSONValue_t));
    GXParseJSON(data, len, rootTokenCount, rootContents);

    // Find and load the textures
    for (size_t k = 0; k < rootTokenCount; k++)
        if (strcmp("albedo", rootContents[k].name) == 0)
            ret->albedo = loadTexture(rootContents[k].content.nWhere);
        else if (strcmp("normal", rootContents[k].name) == 0)
            ret->normal = loadTexture(rootContents[k].content.nWhere);
        else if (strcmp("metallic", rootContents[k].name) == 0)
            ret->metallic = loadTexture(rootContents[k].content.nWhere);
        else if (strcmp("roughness", rootContents[k].name) == 0)
            ret->roughness = loadTexture(rootContents[k].content.nWhere);
        else if (strcmp("AO", rootContents[k].name) == 0)
            ret->AO = loadTexture(rootContents[k].content.nWhere);

    // Free root contents
    free(rootContents);

    // Return the material
    return ret;
}

int assignMaterial ( GXMaterial_t* material, GXShader_t* shader )
{

    // Set the texture uniforms from the shader requested data
    setShaderInt(shader, (const char*) findValue(shader->requestedData, shader->requestedDataCount, GXSP_Albedo) ,0);
    setShaderInt(shader, (const char*) findValue(shader->requestedData, shader->requestedDataCount, GXSP_Normal), 1);
    setShaderInt(shader, (const char*) findValue(shader->requestedData, shader->requestedDataCount, GXSP_Rough) , 2);
    setShaderInt(shader, (const char*) findValue(shader->requestedData, shader->requestedDataCount, GXSP_Metal) , 3);
    setShaderInt(shader, (const char*) findValue(shader->requestedData, shader->requestedDataCount, GXSP_AO)    , 4);

    // Bind the texture units to the textureIDs
    // loadTextureToTextureUnit(material->albedo, )

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, material->albedo->textureID);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, material->normal->textureID);

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, material->metallic->textureID);

    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, material->roughness->textureID);

    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, material->AO->textureID);
    
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
