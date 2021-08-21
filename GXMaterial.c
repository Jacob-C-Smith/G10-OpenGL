#include <G10/GXMaterial.h>

GXTexture_t *missingTexture;

GXMaterial_t* createMaterial ( )
{
    // Initialized data
    GXMaterial_t* ret = calloc(1,sizeof(GXMaterial_t));

    // Check allocated memory
    if (ret == NULL)
        return ret;

    // Zero set the textures
    ret->albedo = (void*)0;
    ret->normal = (void*)0;
    ret->rough  = (void*)0;
    ret->metal  = (void*)0;
    ret->AO     = (void*)0;

    return ret;
}

GXMaterial_t* loadMaterial ( const char path[] )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(path==0)
                goto noPath;
        #endif
    }

    // Uninitialized data
    int          i;
    u8*          data;
    size_t       len,
                 rootTokenCount;

    // Initialized data
    GXMaterial_t* ret          = calloc(1,sizeof(GXMaterial_t));
    FILE*         f            = fopen(path, "rb");  

    #ifndef NDEBUG
        // Check allocated memory
        if (ret == 0)
            return ret;
    #endif

    // Load up the file
    i = gLoadFile(path, 0);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data);

    ret = loadMaterialAsJSON(data);

    free(data);

    // Return the material
    return ret;

    // Error handling
    {
        noPath:
        #ifndef NDEBUG
            gPrintLog("[G10] [Material] No path provided to function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXMaterial_t* loadMaterialAsJSON(char* token)
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
            ret->metal = loadTexture(tokens[k].content.nWhere);
            continue;
        }
        else if (strcmp("rough", tokens[k].name) == 0)
        {
            ret->rough = loadTexture(tokens[k].content.nWhere);
            continue;
        }
        else if (strcmp("AO", tokens[k].name) == 0)
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

GXMaterial_t* getMaterial( GXMaterial_t *materials, const char name[] )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if (materials == 0)
                goto nullMaterials;
            if (name == 0)
                goto nullName;
        #endif
    }

    // Initialized data
    GXMaterial_t* i = materials;

    // Sanity check
    if (i == 0)
        goto noMaterial;
    
    // Iterate through list until we hit the entity we want, or zero
    while (i)
    {
        if (strcmp(name, i->name) == 0)
            return i; // If able to locate the entity in question, return a pointer
        i = i->next;
    }
    
    // Unable to locate entity
    goto noMatch;

    // Error handling
    {
        // There are no entities
        noMaterial:
        #ifndef NDEBUG
            gPrintError("[G10] [Material] There are no materials.\n");
        #endif
        return 0;	

        // There is no match
        noMatch:
        #ifndef NDEBUG
            gPrintError("[G10] [Material] There is no materials named \"%s\".", name);
        #endif
        return 0;
    
        // The scene parameter was null
        nullMaterials:
        #ifndef NDEBUG
            gPrintError("[G10] [Material] Null pointer provided for scene in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            gPrintError("[G10] [Material] Null pointer provided for name in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int assignMaterial ( GXMaterial_t* material, GXShader_t* shader )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if (material == 0)
                goto nullMaterial;
            albedoCheck:
            if (material->albedo == 0)
                goto nullAlbedo;
            normalCheck:
            if (material->normal == 0)
                goto nullNormal;
            roughCheck:
            if (material->rough == 0)
                goto nullRough;
            metalCheck:
            if (material->metal == 0)
                goto nullMetal;
            AOCheck:
            if (material->AO == 0)
                goto nullAO;
        #endif
        endArgumentCheck:;
    }

    //Bind the texture units to the textureIDs
    size_t albedoIndex = loadTextureToTextureUnit(material->albedo);
    size_t normalIndex = loadTextureToTextureUnit(material->normal);
    size_t roughIndex  = loadTextureToTextureUnit(material->rough);
    size_t metalIndex  = loadTextureToTextureUnit(material->metal);
    size_t aoIndex     = loadTextureToTextureUnit(material->AO);

    // Set the texture uniforms from the shader requested data
    setShaderInt(shader, (const char*)findValue(shader->requestedData, shader->requestedDataCount, GXSP_Albedo), albedoIndex);
    setShaderInt(shader, (const char*)findValue(shader->requestedData, shader->requestedDataCount, GXSP_Normal), normalIndex);
    setShaderInt(shader, (const char*)findValue(shader->requestedData, shader->requestedDataCount, GXSP_Rough), roughIndex);
    setShaderInt(shader, (const char*)findValue(shader->requestedData, shader->requestedDataCount, GXSP_Metal), metalIndex);
    setShaderInt(shader, (const char*)findValue(shader->requestedData, shader->requestedDataCount, GXSP_AO), aoIndex);

    return 0;

    // Error handling
    {
        nullMaterial:
            #ifndef NDEBUG
                gPrintError("[G10] [Material] Null pointer provided for parameter \"material\" in function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        nullAlbedo:
            #ifndef NDEBUG
                gPrintError("[G10] [Material] No albedo was available to assign in material \"%s\" from function \"%s\"\n", material->name, __FUNCSIG__);
                gPrintWarning("[G10] [Material] Using missing texture texture instead\n");
                material->albedo = missingTexture;
                goto normalCheck;
            #endif
            return 0;
        nullNormal:
            #ifndef NDEBUG
                gPrintError("[G10] [Material] No normal was available to assign in material \"%s\" from function \"%s\"\n", material->name, __FUNCSIG__);
                gPrintWarning("[G10] [Material] Using missing texture texture instead\n");
                material->normal = missingTexture;
                goto roughCheck;
            #endif
            return 0;
        nullRough:
            #ifndef NDEBUG
                gPrintError("[G10] [Material] No roughness was available to assign in material \"%s\" from function \"%s\"\n", material->name, __FUNCSIG__);
                gPrintWarning("[G10] [Material] Using missing texture texture instead\n");
                material->rough = missingTexture;
                goto metalCheck;
            #endif
                return 0;
        nullMetal:
            #ifndef NDEBUG
                gPrintError("[G10] [Material] No metalness was available to assign in material \"%s\" from  function \"%s\"\n", material->name, __FUNCSIG__);
                gPrintWarning("[G10] [Material] Using missing texture texture instead\n");
                material->metal = missingTexture;
                goto AOCheck;
            #endif
            return 0;           
        nullAO:
            #ifndef NDEBUG
                gPrintError("[G10] [Material] No ambient occlusion was available to assign in material \"%s\" from function \"%s\"\n", material->name, __FUNCSIG__);
                gPrintWarning("[G10] [Material] Using missing texture texture instead\n");
                material->AO = missingTexture;
                goto endArgumentCheck;
            #endif
            return 0;
    }
}

int unloadMaterial ( GXMaterial_t* material )
{
    // Unload all of the textures
    unloadTexture(material->albedo);
    unloadTexture(material->normal);
    unloadTexture(material->metal);
    unloadTexture(material->rough);
    unloadTexture(material->AO);

    // Free the material
    free(material);

    return 0;
}
