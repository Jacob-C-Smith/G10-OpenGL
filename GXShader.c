#include <G10/GXShader.h>

GXShader_t* loadShader(const char shaderPath[])
{
    // Uninitialized data
    int          i;
    char        *data;
    GXShader_t  *ret;
    size_t       len,
                 rootTokenCount;
    JSONValue_t *tokens;

    // Initialized data
    FILE         *f                  = fopen(shaderPath, "rb");


    // Load the file
    {
        // Check if file is valid
        if (f == NULL)
        {
            gPrintError("[G10] [Shader] Failed to load file %s\n", shaderPath);
            return (void*)0;
        }

        // Find file size and prep for read
        fseek(f, 0, SEEK_END);
        i = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Allocate data and read file into memory
        data = malloc(i+1);

        // Check if data is valid
        if (data == 0)
            return (void*)0;

        // Read to data
        fread(data, 1, i, f);

        // We no longer need the file
        fclose(f);

        data[i] = '\0';
    }

    ret = loadShaderAsJSON(data);


}

GXShader_t* loadShaderAsJSON(char* token)
{
    // Uninitialized data
    int           i;
    char*         data;
    GXShader_t*   ret;
    size_t        len,
                  rootTokenCount;
    JSONValue_t*  tokens;

    // Initialized data
    char        * vertexShaderPath   = 0,
                * fragmentShaderPath = 0,
                * shaderName         = 0;
    GXKeyValue_t* requestedData      = 0;
    size_t        requestedDataCount = 0,
                  requestedDataFlags = 0;

    // Preparse JSON
    {
        len            = strlen(token),
        rootTokenCount = GXParseJSON(token, len, 0, 0);
        tokens         = calloc(rootTokenCount, sizeof(JSONValue_t));
    }

    // Parse JSON Values
    GXParseJSON(token, len, rootTokenCount, tokens);

    // Find and load the shaders and token
    for (size_t j = 0; j < rootTokenCount; j++)
    {
        // Point to the vertex shader
        if (strcmp("vertexShaderPath", tokens[j].name) == 0)
        {
            vertexShaderPath = tokens[j].content.nWhere;
            continue;
        }

        // Point to the fragment shader
        else if (strcmp("fragmentShaderPath", tokens[j].name) == 0)
        {
            fragmentShaderPath = tokens[j].content.nWhere;
            continue;
        }

        // Copy out the name of the shader
        else if (strcmp("name", tokens[j].name) == 0)
        {
            shaderName = tokens[j].content.nWhere;
            continue;
        }

        // Copy out requested data
        else if (strcmp("requestData", tokens[j].name) == 0)
        {
            // Uninitialized data
            JSONValue_t* subContents;
            size_t       subTokenCount,
                         vLen;

            // Initialized data
            size_t       len   = strlen(tokens[j].content.nWhere);
            
            // Preparse the data
            subTokenCount      = GXParseJSON(tokens[j].content.nWhere, len, 0, 0);
            subContents        = calloc(subTokenCount, sizeof(JSONValue_t));

            // Allocate space for the key/value pairs
            requestedData      = calloc(subTokenCount, sizeof(GXKeyValue_t));
            requestedDataCount = subTokenCount;

            if (requestedData == (void*)0)
                return 0;

            // Parse out JSON light data
            GXParseJSON(tokens[j].content.nWhere, len, subTokenCount, subContents);

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
                        gPrintWarning("[G10] [Shader] Shader requested \"%s\", which is unsupported data.", subContents[k].name);
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
        ret                     = loadCompileShader(vertexShaderPath, fragmentShaderPath, shaderName);
        ret->name               = shaderName;
        ret->requestedDataFlags = requestedDataFlags;
        ret->requestedData      = requestedData;
        ret->requestedDataCount = requestedDataCount;
    }

    // Free subcontents
    free(tokens);

    // Set the shader
    return ret;
}

GXShader_t* loadCompileShader ( const char vertexShaderPath[], const char fragmentShaderPath[], const char shaderName[] )
{
    // Uninitialized data
    char       * vfdata,                               // Vertex shader text
               * ffdata;                               // Fragment shader text
    size_t       vfi,                                  // Vertex shader text index
                 ffi;                                  // Fragment shader text index
    unsigned int vShader,                              // OpenGL vertex shader
                 fShader;                              // OpenGL fragment shader
    int          status;                               // Checks for compilation issues

    // Initialized data
    FILE* vf        = fopen(vertexShaderPath, "rb");   // Vertex shader source code FILE
    FILE* ff        = fopen(fragmentShaderPath, "rb"); // Fragment shader source code FILE
    
    GXShader_t* ret = malloc(sizeof(GXShader_t));      // The return 


    if (ret == 0)
        return (void*)0;

    // Load the files
    {
        // Check files
        if (vf == NULL)
        {
            gPrintError("[G10] [Shader] Failed to load file %s\n", vertexShaderPath);
            return (void*)0;
        }

        if (ff == NULL)
        {
            gPrintError("[G10] [Shader] Failed to load file %s\n", fragmentShaderPath);
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
        vfdata = calloc(vfi+1,sizeof(char));
        ffdata = calloc(ffi+1, sizeof(char));

        fread(vfdata, 1, vfi, vf);
        fread(ffdata, 1, ffi, ff);

        // We no longer need the file
        fclose(vf);
        fclose(ff);

        vfdata[vfi] = '\0';
        ffdata[ffi] = '\0';
    }

    compileFromText(ret, vfdata, ffdata);

    // We no longer need the shader text
    free(vfdata);
    free(ffdata);

    // We aren't requesting data yet, so we will just leave it as nullptr,
    // in case the shader never requests data.
    ret->requestedData = 0;

    return ret;

}

int compileFromText ( GXShader_t* shader, char* vertexShaderText, char* fragmentShaderText )
{
    // TODO: Argument check, error handling

    // Uninitialized data
    unsigned int vShader, // OpenGL vertex shader
                 fShader; // OpenGL fragment shader

    // Initialized data
    int          status = 0;

    // Compile, attach, and link shaders
    vShader = glCreateShader(GL_VERTEX_SHADER);
    fShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vShader, 1, &vertexShaderText, NULL);
    glShaderSource(fShader, 1, &fragmentShaderText, NULL);

    glCompileShader(vShader);
    glCompileShader(fShader);

    // Check to make sure the vertex shader compiled without errors
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

    // Clear status and check to make sure the fragment shader compiled without errors  
    status ^= 0;
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

    // Create a shader program
    shader->shaderProgramID = glCreateProgram();

    // Attach both of the shaders 
    glAttachShader(shader->shaderProgramID, vShader);
    glAttachShader(shader->shaderProgramID, fShader);

    // Link the program
    glLinkProgram(shader->shaderProgramID);

    // Clear status and check to make sure the shader program compiled correctly
    status ^= 0;

    glGetProgramiv(shader->shaderProgramID, GL_LINK_STATUS, &status);
    if (!status)
    {
        char* log = malloc(512);
        if (log == 0)
            return (void*)0;
        glGetProgramInfoLog(shader->shaderProgramID, 512, NULL, log);
        printf(log);
        free(log);
    }

    // Detach the shaders
    glDetachShader(shader->shaderProgramID, vShader);
    glDetachShader(shader->shaderProgramID, fShader);

    // Destroy the shader programs we don't need anymore
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    return 0;
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