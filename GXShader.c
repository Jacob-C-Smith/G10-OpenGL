#include <G10/GXShader.h>

GXShader_t* loadShader ( const char vertexShaderPath[], const char fragmentShaderPath[], const char shaderName[] )
{
    // Uninitialized data
    char*        vfdata ,* ffdata;
    int          vfi    ,  ffi;
    unsigned int vShader, fShader;
    int          status;

    // Initialized data
    FILE* vf        = fopen(vertexShaderPath, "rb");
    FILE* ff        = fopen(fragmentShaderPath, "rb");
    
    GXShader_t* ret = malloc(sizeof(GXShader_t));
    if (ret == 0)
        return (void*)0;

    // Check files
    if (vf == NULL)
    {
        printf("Failed to load file %s\n", vertexShaderPath);
        return (void*)0;
    }

    if (ff == NULL)
    {
        printf("Failed to load file %s\n", fragmentShaderPath);
        return (void*)0;
    }

    // Find file size and prep for read
    fseek(vf, 0, SEEK_END);
    fseek(ff, 0, SEEK_END);

    vfi = ftell(vf);
    ffi = ftell(ff);

    fseek(vf, 0, SEEK_SET);
    fseek(ff, 0, SEEK_SET);

    // Allocate data and read file into memory
    vfdata = malloc(vfi+1);
    ffdata = malloc(ffi+1);
    if (vfdata == 0)
        return (void*)0;
    if (ffdata == 0)
        return (void*)0;

    fread(vfdata, 1, vfi, vf);
    fread(ffdata, 1, ffi, ff);

    // We no longer need the file
    fclose(vf);
    fclose(ff);

    vfdata[vfi] = '\0';
    ffdata[ffi] = '\0';

    // Compile, attach, and link shaders
    vShader = glCreateShader(GL_VERTEX_SHADER);
    fShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vShader, 1, &vfdata, NULL);
    glShaderSource(fShader, 1, &ffdata, NULL);

    glCompileShader(vShader);
    glCompileShader(fShader);

    glGetShaderiv(vShader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        char* log = malloc(512);
        if (log == 0)
            return (void*)0;
        glGetShaderInfoLog(vShader, 512, NULL, log);
        printf(log);
        free(log);
    }

    status = 0;
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        char* log = malloc(512);
        if (log == 0)
            return (void*)0;
        glGetShaderInfoLog(fShader, 512, NULL, log);
        printf(log);
        free(log);
    }

    ret->shaderProgramID = glCreateProgram();

    glAttachShader(ret->shaderProgramID, vShader);
    glAttachShader(ret->shaderProgramID, fShader);

    glLinkProgram(ret->shaderProgramID);

    status = 0;

    glGetProgramiv(ret->shaderProgramID, GL_LINK_STATUS, &status);
    if (!status)
    {
        char* log = malloc(512);
        if (log == 0)
            return (void*)0;
        glGetProgramInfoLog(ret->shaderProgramID, 512, NULL, log);
        printf(log);
        free(log);
    }
    glDetachShader(ret->shaderProgramID, vShader);
    glDetachShader(ret->shaderProgramID, fShader);

    // Destroy the shader programs we don't need anymore
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    // We no longer need the shader text
    free(vfdata);
    free(ffdata);

    // We aren't requesting data yet, so we will just leave it as nullptr,
    // in case the shader never requests data.
    ret->requestedData = 0;

    return ret;

}

GXShader_t* loadShaderAsJSON(const char shaderPath[])
{
    // Uninitialized data
    int           i;
    char*         data;
    GXShader_t*   ret;
    size_t        len,
                  rootTokenCount;
    JSONValue_t*  rootContents;

    // Initialized data
    FILE        * f                  = fopen(shaderPath, "rb");
    char        * vertexShaderPath   = 0,
                * fragmentShaderPath = 0,
                * shaderName         = 0;
    GXKeyValue_t* requestedData      = 0;
    size_t        requestedDataCount = 0,
                  requestedDataFlags = 0;

    // Load the file
    {
        // Check if file is valid
        if (f == NULL)
        {
            printf("Failed to load file %s\n", shaderPath);
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

        data[i] = '\0';
    }

    // Preparse JSON
    {
        len            = strlen(data),
        rootTokenCount = GXParseJSON(data, len, 0, 0);
        rootContents   = calloc(rootTokenCount, sizeof(JSONValue_t));
    }

    // Parse JSON Values
    GXParseJSON(data, len, rootTokenCount, rootContents);

    // Find and load the shaders and data
    for (size_t j = 0; j < rootTokenCount; j++)
    {
        // Point to the vertex shader
        if (strcmp("vertexShaderPath", rootContents[j].name) == 0)
        {
            vertexShaderPath = rootContents[j].content.nWhere;
            continue;
        }

        // Point to the fragment shader
        else if (strcmp("fragmentShaderPath", rootContents[j].name) == 0)
        {
            fragmentShaderPath = rootContents[j].content.nWhere;
            continue;
        }

        // Copy out the name of the shader
        else if (strcmp("name", rootContents[j].name) == 0)
        {
            shaderName = rootContents[j].content.nWhere;
            continue;
        }

        // Copy out requested data
        else if (strcmp("requestData", rootContents[j].name) == 0)
        {
            // Uninitialized data
            JSONValue_t* subContents;
            size_t       subTokenCount,
                         vLen;

            // Initialized data
            size_t       len   = strlen(rootContents[j].content.nWhere);
            
            // Preparse the data
            subTokenCount      = GXParseJSON(rootContents[j].content.nWhere, len, 0, 0);
            subContents        = calloc(subTokenCount, sizeof(JSONValue_t));

            // Allocate space for the key/value pairs
            requestedData      = calloc(subTokenCount, sizeof(GXKeyValue_t));
            requestedDataCount = subTokenCount;

            if (requestedData == (void*)0)
                return 0;

            // Parse out JSON light data
            GXParseJSON(rootContents[j].content.nWhere, len, subTokenCount, subContents);

            // Iterate through key / value pairs to find relevent information
            for (size_t k = 0; k < subTokenCount; k++)
            {

                // Parse out the value of the view matrix
                if (strcmp("view", subContents[k].name) == 0)
                {
                    requestedData[k].key = GXSP_View;
                    requestedDataFlags  |= GXSP_View;
                    goto setValue;
                }

                // Parse out the value of the projection matrix
                else if (strcmp("projection", subContents[k].name) == 0)
                {
                    requestedData[k].key = GXSP_Projection;
                    requestedDataFlags  |= GXSP_Projection;
                    goto setValue;
                }

                // Parse out the value of the model matrix
                else if (strcmp("model", subContents[k].name) == 0)
                {
                    requestedData[k].key = GXSP_Model;
                    requestedDataFlags  |= GXSP_Model;
                    goto setValue;
                }

                // Parse out the value of the camera position
                else if (strcmp("camera position", subContents[k].name) == 0)
                {
                    requestedData[k].key = GXSP_CameraPosition;
                    requestedDataFlags  |= GXSP_CameraPosition;
                    goto setValue;
                }

                // Parse out the value of the light position
                else if (strcmp("light position", subContents[k].name) == 0)
                {
                    requestedData[k].key = GXSP_LightPosition;
                    requestedDataFlags  |= GXSP_LightPosition;
                    goto setValue;
                }

                // Parse out the value of the light color
                else if (strcmp("light color", subContents[k].name) == 0)
                {
                    requestedData[k].key = GXSP_LightColor;
                    requestedDataFlags  |= GXSP_LightColor;
                    goto setValue;
                }

                // Parse out the value of the light count
                else if (strcmp("light count", subContents[k].name) == 0)
                {
                    requestedData[k].key = GXSP_LightCount;
                    requestedDataFlags  |= GXSP_LightCount;
                    goto setValue;
                }

                // Parse out the value of the albedo texture
                else if (strcmp("albedo", subContents[k].name) == 0)
                {
                    requestedData[k].key = GXSP_Albedo;
                    requestedDataFlags  |= GXSP_Albedo;
                    goto setValue;
                }

                // Parse out the value of the normal texture
                else if (strcmp("normal", subContents[k].name) == 0)
                {
                    requestedData[k].key = GXSP_Normal;
                    requestedDataFlags  |= GXSP_Normal;
                    goto setValue;
                }

                // Parse out the value of the metallic texture
                else if (strcmp("metal", subContents[k].name) == 0)
                {
                    requestedData[k].key = GXSP_Metal;
                    requestedDataFlags  |= GXSP_Metal;
                    goto setValue;
                }

                // Parse out the value of the roughness map
                else if (strcmp("rough", subContents[k].name) == 0)
                {
                    requestedData[k].key = GXSP_Rough;
                    requestedDataFlags  |= GXSP_Rough;
                    goto setValue;
                }

                // Parse out the value of the ao map
                else if (strcmp("ao", subContents[k].name) == 0)
                {
                    requestedData[k].key = GXSP_AO;
                    requestedDataFlags  |= GXSP_AO;
                    goto setValue;
                }

                // If the key is unsupported, we log, ignore, and keep looking for new pairs.
                else {
                    #ifndef NDEBUG
                        printf("Shader requested \"%s\", which is unsupported data.", subContents[k].name);
                    #endif
                    continue;
                }

                setValue:
                {
                    vLen = strlen(subContents[k].content.nWhere) + 1;

                    // Set the value 
                    requestedData[k].value = malloc(vLen);
                    if (requestedData[k].value == (void*)0)
                        return 0;

                    strncpy(requestedData[k].value, subContents[k].content.nWhere, vLen);
                    requestedData[k].value[vLen + 1] = 0; 
                }
            }

            // Free subcontents
            free(subContents);

            continue;
        }
    }

    // Spin up the shader, set the name and the requested data
    {
        ret                     = loadShader(vertexShaderPath, fragmentShaderPath, shaderName);
        ret->name               = shaderName;
        ret->requestedDataFlags = requestedDataFlags;
        ret->requestedData      = requestedData;
        ret->requestedDataCount = requestedDataCount;
    }

    // Free subcontents
    free(rootContents);

    // Set the shader
    return ret;
}

int useShader ( GXShader_t* shader )
{
    if(shader)
        glUseProgram(shader->shaderProgramID);

    return 0;
}

void setShaderInt ( GXShader_t* shader, const char name[], int value )
{
    glUniform1i(glGetUniformLocation(shader->shaderProgramID, name),value);
}

void setShaderFloat ( GXShader_t* shader, const char name[], float value )
{
    glUniform1f(glGetUniformLocation(shader->shaderProgramID, name),value);
}

void setShaderVec3 ( GXShader_t* shader, const char name[], GXvec3_t vector )
{
    glUniform3f(glGetUniformLocation(shader->shaderProgramID, name), vector.x, vector.y, vector.z);
}

void setShaderMat4 ( GXShader_t* shader, const char name[], GXmat4_t* m )
{
    glUniformMatrix4fv(glGetUniformLocation(shader->shaderProgramID, name), 1, GL_FALSE, m);
}

int unloadShader ( GXShader_t* shader )
{
    // Deallocate associated data
    glDeleteProgram(shader->shaderProgramID);
    shader->shaderProgramID = 0;
    free(shader);

    return 0;
}