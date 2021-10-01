#include <G10/GXShader.h>

GXShader_t* createShader()
{
    GXShader_t* ret = calloc(1, sizeof(GXShader_t));

    return ret;
}

GXShader_t* loadShader(const char path[])
{
    // Uninitialized data
    size_t       i;
    char        *data;
    GXShader_t  *ret;
    size_t       len,
                 rootTokenCount;
    JSONValue_t *tokens;

    // Initialized data
    FILE        *f = fopen(path, "rb");


    // Load the file
    i    = gLoadFile(path, 0);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data);

    ret = loadShaderAsJSON(data);

    #ifndef NDEBUG
        if(ret == (void*)0)
            goto badFile;
    #endif
    
    return ret;

    // Error handling
    {
    badFile:
        #ifndef NDEBUG
            gPrintError("[G10] [Shader] Unable to parse \"%s\"\n",path);
        #endif
        return 0;
    }
}

GXShader_t* loadShaderAsJSON(char* token)
{
    // Uninitialized data
    int                i;
    char              *data;
    size_t             len,
                       rootTokenCount;
    JSONValue_t       *tokens;

    // Initialized data
    GXShader_t        *ret                = createShader();
    char              *vertexPath         = 0,
                      *fragmentPath       = 0,
                      *shaderName         = 0;
    GXUniform_t       *requestedData      = 0;
    size_t             requestedDataCount = 0, 
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
        if (strcmp("vertex shader path", tokens[j].name) == 0)
        {
            vertexPath = tokens[j].content.nWhere;
            continue;
        }

        // Point to the fragment shader
        else if (strcmp("fragment shader path", tokens[j].name) == 0)
        {
            fragmentPath = tokens[j].content.nWhere;
            continue;
        }

        // Copy out the name of the shader
        else if (strcmp("name", tokens[j].name) == 0)
        {
            shaderName = tokens[j].content.nWhere;
            continue;
        }

        // Copy out requested data
        else if (strcmp("uniforms", tokens[j].name) == 0)
        {
            requestedData = loadUniformAsJSON(tokens[j].content.aWhere[0]);
            for ( requestedDataCount = 1; tokens[j].content.aWhere[requestedDataCount]; requestedDataCount++)
                appendUniform(requestedData, loadUniformAsJSON(tokens[j].content.aWhere[requestedDataCount]));



            continue;
        }
    }

    // Spin up the shader, set the name and the requested data
    {
        ret                     = loadCompileShader(vertexPath, fragmentPath, shaderName);
        ret->name               = shaderName;
        ret->requestedDataCount = requestedDataCount;
        ret->requestedData      = requestedData;
    }

    // Free subcontents
    free(tokens);

    // Set the shader
    return ret;
}

GXUniform_t *loadUniformAsJSON( char *token )
{
    // Uninitialized data
    int                i;
    char              *data;
    GXUniform_t       *ret = calloc(1,sizeof(GXUniform_t));
    size_t             len,
                       rootTokenCount;
    JSONValue_t       *tokens;

    // Initialized data
    char              *name         = 0,
                      *uniformName  = 0;
    u32                type         = 0;
                       
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
        if (strcmp(tokens[j].name, "name") == 0)
            name = tokens[j].content.nWhere;
        if (strcmp(tokens[j].name, "type") == 0)
            if (tokens[j].type == GXJSONarray)
                ret->type = GXUNIFSTRUCT;
            else
                ret->type = *(u32*)tokens[j].content.nWhere;
        if (strcmp(tokens[j].name, "uniform name") == 0)
            uniformName = tokens[j].content.nWhere;
    }

    if (name && uniformName)
        ret->keyValue = createKeyValue(name, uniformName);
    else
        return 0;
    return ret;

    // Error handling
    {

        unknownUniformType:
        #ifndef NDEBUG
            gPrintError("[G10] [Shader] Unknown uniform type ");
        #endif
        return 0;
    }
}

int appendUniform(GXUniform_t *list, GXUniform_t* uniform)
{
    // Argument checking
    {
        #ifndef NDEBUG
            if(uniform == 0)
                goto nullUniform;
        #endif
    }

    // Set the pointer to the head of the linked list
    GXUniform_t* i = list;

    // Check if the head is null. If so, set the head to the mesh
    if (i == 0)
    {
        list = uniform;
        return 0;
    }

    // Search for the end of the linked list, and check every mesh 
    while (i->next)
    {
        // Error checking
        if (strcmp(i->keyValue->key, uniform->keyValue->key) == 0)
            goto duplicateName;
        i = i->next;
    }

    // Assign next as entity
    i->next = uniform;

    return 0;

    // Error handling
    {
        // Two parts with the same name cannot exist in the same mesh
        duplicateName:
        #ifndef NDEBUG
            gPrintError("[G10] [Shader] Uniform \"%s\" can not be appended because a uniform with that name already exists\n", uniform->keyValue->key);
        #endif
        return 0;

        // The part argument was null
        nullUniform:
        #ifndef NDEBUG
            gPrintError("[G10] [Shader] Null pointer provided for \"uniform\" in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXShader_t* loadCompileShader ( const char vertexPath[], const char fragmentPath[], const char shaderName[] )
{
    // Uninitialized data
    char       * vfdata,                            // Vertex shader text
               * ffdata;                            // Fragment shader text
    size_t       vfi,                               // Vertex shader text index
                 ffi;                               // Fragment shader text index
    unsigned int vShader,                           // OpenGL vertex shader
                 fShader;                           // OpenGL fragment shader
    int          status;                            // Checks for compilation issues

    // Initialized data
    FILE* vf        = fopen(vertexPath, "rb");      // Vertex shader source code FILE
    FILE* ff        = fopen(fragmentPath, "rb");    // Fragment shader source code FILE
    
    GXShader_t* ret = calloc(1,sizeof(GXShader_t)); // The return 

    // TODO: Skip to error handling
    #ifndef NDEBUG
        if (ret == 0)
            return (void*)0;
    #endif

    // Load the files
    {
        // Check files
        if (vf == NULL)
        {
            gPrintError("[G10] [Shader] Failed to load file %s\n", vertexPath);
            return (void*)0;
        }

        if (ff == NULL)
        {
            gPrintError("[G10] [Shader] Failed to load file %s\n", fragmentPath);
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
        char* log = calloc(512,sizeof(u8));
        if (log == 0)
            return 0;
        glGetShaderInfoLog(vShader, 512, NULL, log);
        printf(log);
        free(log);
    }

    // Clear status and check to make sure the fragment shader compiled without errors  
    status ^= 0;
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        char* log = calloc(512,sizeof(u8));
        if (log == 0)
            return 0;
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
        char* log = calloc(512,sizeof(u8));
        if (log == 0)
            return 0;
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

void setShaderInt ( GXShader_t* shader, const char *name, int value )
{
    glUniform1i(glGetUniformLocation(shader->shaderProgramID, name),value);
}

void setShaderFloat ( GXShader_t* shader, const char *name, float value )
{
    glUniform1f(glGetUniformLocation(shader->shaderProgramID, name),value);
}

void setShaderVec3 ( GXShader_t* shader, const char *name, vec3 vector )
{
    glUniform3f(glGetUniformLocation(shader->shaderProgramID, name), vector.x, vector.y, vector.z);
}

void setShaderMat4 ( GXShader_t* shader, const char *name, mat4* m )
{
    glUniformMatrix4fv(glGetUniformLocation(shader->shaderProgramID, name), 1, GL_FALSE, (const float*)m);
}

void setShaderTexture ( GXShader_t* shader, const char* name, GXTexture_t* texture)
{
    setShaderInt(shader, name, loadTextureToTextureUnit(texture));
}

void setShaderTransform(GXShader_t* shader, GXTransform_t* transform)
{
    GXUniform_t* i = shader->requestedData;

    for (size_t j = 0; j < shader->requestedDataCount; j++)
    {
        if (strcmp(i->keyValue->key, "model") == 0)
            setShaderMat4(shader, i->keyValue->value, &transform->modelMatrix);
        i = i->next;
    }
}

void setShaderCamera ( GXShader_t* shader, GXCamera_t* camera)
{
    GXUniform_t *i = shader->requestedData;

    for (size_t j = 0; j < shader->requestedDataCount; j++)
    {
        if (strcmp(i->keyValue->key,"camera position") == 0)
            setShaderVec3(shader, i->keyValue->value, camera->where);
        if (strcmp(i->keyValue->key, "view") == 0) {
            setShaderMat4(shader, i->keyValue->value, &camera->view);
        }
        if (strcmp(i->keyValue->key, "projection") == 0)
        {
            setShaderMat4(shader, i->keyValue->value, &camera->projection);
        }
        i = i->next;
    }
}

void setCameraLights(GXShader_t* shader, const char *name, GXLight_t* light)
{

}

int setShaderMaterial ( GXShader_t* shader, GXMaterial_t* material )
{

}

int unloadShader ( GXShader_t* shader )
{
    // Deallocate associated data
    glDeleteProgram(shader->shaderProgramID);
    shader->shaderProgramID = 0;
    free(shader);

    return 0;
}

