#include <G10/GXShader.h>

GXShader_t  *create_shader        ( void )
{
    GXShader_t* ret = calloc(1, sizeof(GXShader_t));
    
    // Check the memory
    {
        #ifndef NDEBUG
            if (ret == 0)
                goto noMem;
        #endif
    }

    return ret;

    // Error handling
    {
        noMem:
        #ifndef NDEBUG
            g_print_error("[G10] [Shader] Out of memory.\n");
        #endif
        return 0;
    }
}

GXShader_t  *load_shader          ( const char   path[])
{

    // Argument check
    {
        if (path == (void*)0)
            goto no_path;
    }

    // Uninitialized data
    size_t       i;
    char        *data;
    GXShader_t  *ret;
    size_t       len,
                 rootToken_count;
    JSONToken_t *tokens;

    // Load the file
    i    = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    ret = load_shader_as_json(data);

    free(data);
    
    return ret;

    // Error handling
    {

        // Argument errors
        {
            no_path:
            #ifndef NDEBUG
                g_print_error("[G10] [Shader] Null pointer provided for \"path\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        }
    }
}

GXShader_t  *load_shader_as_json  ( char        *token)
{

    // Argument check
    {
        if(token == (void *)0)
            goto no_token;
    }

    // Uninitialized data
    int                i;
    char              *data;
    size_t             len,
                       rootToken_count;
    JSONToken_t       *tokens;

    // Initialized data
    GXShader_t        *ret                = create_shader();
    char              *vertexPath         = 0,
                      *fragmentPath       = 0,
                      *shaderName         = 0;
    GXUniform_t       *requestedData      = 0;
    size_t             requestedDataCount = 0, 
                       requestedDataFlags = 0;

    // Preparse JSON
    {
        len            = strlen(token),
        rootToken_count = parse_json(token, len, 0, 0);
        tokens         = calloc(rootToken_count, sizeof(JSONToken_t));
    }

    // Parse JSON Values
    parse_json(token, len, rootToken_count, tokens);

    // Find and load the shaders and token
    for (size_t j = 0; j < rootToken_count; j++)
    {

        // Copy out the name of the shader
        if (strcmp("name", tokens[j].key) == 0)
        {
            GXShader_t* p_ret = g_find_shader(g_get_active_instance(), tokens[j].value.n_where);

            if (p_ret)
            {
                destroy_shader(ret);
                ret = p_ret;
                #ifndef NDEBUG
                    g_print_log("[G10] [Shader] Shader \"%s\" loaded from cache\n", p_ret->name);
                #endif
                goto exit_cache;
            }


            char* s_name = tokens[j].value.n_where;
            size_t l = strlen(s_name);
            ret->name = calloc(l + 1, sizeof(u8));

            strncpy(ret->name, s_name, l);

            continue;
        }

        // Point to the vertex shader
        else if (strcmp("vertex shader path", tokens[j].key) == 0)
        {
            vertexPath = tokens[j].value.n_where;
            continue;
        }

        // Point to the fragment shader
        else if (strcmp("fragment shader path", tokens[j].key) == 0)
        {
            fragmentPath = tokens[j].value.n_where;
            continue;
        }



        // Copy out requested data
        else if (strcmp("uniforms", tokens[j].key) == 0)
        {
            requestedData = load_uniform_as_json(tokens[j].value.a_where[0]);
            for ( requestedDataCount = 1; tokens[j].value.a_where[requestedDataCount]; requestedDataCount++)
                appendUniform(requestedData, load_uniform_as_json(tokens[j].value.a_where[requestedDataCount]));



            continue;
        }
    }

    // Spin up the shader, set the name and the requested data
    {
        load_compile_shader(ret, vertexPath, fragmentPath);
        ret->requested_data_count = requestedDataCount;
        ret->requested_data       = requestedData;
    }

    // Cache the shader
    g_cache_shader(g_get_active_instance(), ret);

    exit_cache:
        ret->users++;

    // Free subcontents
    free(tokens);

    // Set the shader
    return ret;

    // Error handling
    {

        // Argument errors
        {
            no_token:
            #ifndef NDEBUG
                g_print_error("[G10] [Shader] Null pointer provided for \"token\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        }


    }

}

GXUniform_t *load_uniform_as_json ( char        *token )
{
    // Argument check
    {
        if (token == (void*)0)
            goto no_token;
    }

    // Uninitialized data
    int                i;
    char              *data;
    size_t             len,
                       rootToken_count;
    JSONToken_t       *tokens;

    // Initialized data
    GXUniform_t       *ret          = calloc(1,sizeof(GXUniform_t));
    char              *name         = 0,
                      *uniformName  = 0;
    u32                type         = 0;
                       
    // Preparse JSON
    {
        len            = strlen(token),
        rootToken_count = parse_json(token, len, 0, 0);
        tokens         = calloc(rootToken_count, sizeof(JSONToken_t));
    }

    // Parse JSON Values
    parse_json(token, len, rootToken_count, tokens);

    // Find and load the shaders and token
    for (size_t j = 0; j < rootToken_count; j++)
    {
        if (strcmp(tokens[j].key, "name") == 0)
        {

            // Allocate for the string and copy it 
            char  *name = tokens[j].value.n_where;
            size_t len  = strlen(name);
            ret->key    = calloc(len+1, sizeof(char));

            strncpy(ret->key, name, len);
            continue;
        }

        if (strcmp(tokens[j].key, "type") == 0)
        {
            if (tokens[j].type == JSONarray)
                ret->type = GXUNIFSTRUCT;
            else
                ret->type = *(u32*)tokens[j].value.n_where;
            continue;
        }
        if (strcmp(tokens[j].key, "uniform name") == 0)
        {

            // Allocate for the string and copy it 
            char   *uniformName = tokens[j].value.n_where;
            size_t  len         = strlen(uniformName);
            ret->value = calloc(len + 1, sizeof(char));

            strncpy(ret->value, uniformName, len);

            continue;
        }
    }

    if (!(ret->key || ret->value))
        return 0;

    free(tokens);

    return ret;

    // Error handling
    {
        // Argument errors
        {
            no_token:
            #ifndef NDEBUG
                g_print_error("[G10] [Shader] Null pointer provided for \"token\" in call to function \"%s\"\n", __FUNCSIG__);
            #endif
            return 0;
        }

        // Uniform errors
        {
            unknownUniformType:
            #ifndef NDEBUG
                g_print_error("[G10] [Shader] Unknown uniform type ");
            #endif
            return 0;
        }
    }
}

int          appendUniform        ( GXUniform_t *list,         GXUniform_t   *uniform )
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
        if (strcmp(i->key, uniform->key) == 0)
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
            g_print_error("[G10] [Shader] Uniform \"%s\" can not be appended because a uniform with that name already exists\n", uniform->key);
        #endif
        return 0;

        // The part argument was null
        nullUniform:
        #ifndef NDEBUG
            g_print_error("[G10] [Shader] Null pointer provided for \"uniform\" in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int     load_compile_shader  ( GXShader_t* shader, char *vertex_path, char *fragment_path)
{
    // Argument check
    {
        #ifndef NDEBUG
            if (shader == (void*)0)
                goto no_shader;
            if (vertex_path == (void*)0)
                goto no_vertex_path;
            if (fragment_path == (void*)0)
                goto no_fragment_path;
        #endif
    }

    // Uninitialized data
    char        *vfdata,                            // Vertex shader text
                *ffdata;                            // Fragment shader text
    size_t       vfi = 0,                           // Vertex shader text index
                 ffi = 0;                           // Fragment shader text index
    unsigned int vShader,                           // OpenGL vertex shader
                 fShader;                           // OpenGL fragment shader
    int          status;                            // Checks for compilation issues

    // Load the files
    vfi = g_load_file(vertex_path, 0, false);
    vfdata = calloc(vfi, sizeof(u8));
    g_load_file(vertex_path, vfdata, false);

    ffi = g_load_file(fragment_path, 0, false);
    ffdata = calloc(ffi, sizeof(u8));
    g_load_file(fragment_path, ffdata, false);

    // Compile shaders
    compile_from_text(shader, vfdata, ffdata);

    // The shader text is no longer needed
    free(vfdata);
    free(ffdata);

    return 0;

    // Error handling
    {
        // Argument errors
        {
            no_shader:
            #ifndef NDEBUG
                g_print_error("[G10] [Shader] Null pointer provided for \"shader\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;

            no_vertex_path:
            #ifndef NDEBUG
                g_print_error("[G10] [Shader] Null pointer provided for \"vertex_path\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
            
            no_fragment_path:
            #ifndef NDEBUG
                g_print_error("[G10] [Shader] Null pointer provided for \"fragment_path\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        }
    }
}

int          compile_from_text    ( GXShader_t  *shader,       char          *vertex_shader_text, char         *fragment_shader_text)
{

    // Argument check
    {
        if(shader == (void *)0)
            goto no_shader;
        if (vertex_shader_text == (void*)0)
            goto no_vertex_shader_text;
        if (fragment_shader_text == (void*)0)
            goto no_fragment_shader_text;
    }

    // Uninitialized data
    unsigned int vShader, // OpenGL vertex shader
                 fShader; // OpenGL fragment shader

    // Initialized data
    int          status = 0;

    // Compile, attach, and link shaders
    vShader = glCreateShader(GL_VERTEX_SHADER);
    fShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vShader, 1, &vertex_shader_text, NULL);
    glShaderSource(fShader, 1, &fragment_shader_text, NULL);

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
    shader->shader_program_id = glCreateProgram();

    // Attach both of the shaders 
    glAttachShader(shader->shader_program_id, vShader);
    glAttachShader(shader->shader_program_id, fShader);

    // Link the program
    glLinkProgram(shader->shader_program_id);

    // Clear status and check to make sure the shader program compiled correctly
    status ^= 0;

    glGetProgramiv(shader->shader_program_id, GL_LINK_STATUS, &status);
    if (!status)
    {
        char* log = calloc(512,sizeof(u8));
        if (log == 0)
            return 0;
        glGetProgramInfoLog(shader->shader_program_id, 512, NULL, log);
        printf(log);
        free(log);
    }

    // Detach the shaders
    glDetachShader(shader->shader_program_id, vShader);
    glDetachShader(shader->shader_program_id, fShader);

    // Destroy the shader object code
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    return 0;

    // Error handling
    {
        // Argument errors
        {
        no_shader:
            #ifndef NDEBUG
                g_print_error("[G10] [Shader] Null pointer provided for \"shader\" in call to \"%s\"\n",__FUNCSIG__);
            #endif  
            return 0;
        no_vertex_shader_text:
            #ifndef NDEBUG
                g_print_error("[G10] [Shader] Null pointer provided for \"vertex_shader_text\" in call to \"%s\"\n",__FUNCSIG__);
            #endif  
            return 0;
        no_fragment_shader_text:
            #ifndef NDEBUG
                g_print_error("[G10] [Shader] Null pointer provided for \"fragment_shader_text\" in call to \"%s\"\n",__FUNCSIG__);
            #endif  
            return 0;
        }
    }
}

int          use_shader           ( GXShader_t  *shader )
{
    // Argument check
    {
        #ifndef NDEBUG
            if (shader == (void*)0)
                goto noShader;
        #endif
    }

    glUseProgram(shader->shader_program_id);

    return 1;

    // Error handling
    {
    noShader:
        #ifndef NDEBUG
            g_print_error("[G10] [Shader] Null pointer provided for \"shader\" in function %s\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

void         set_shader_int       ( GXShader_t  *shader,       const char    *name,              int          value )
{
    // TODO: Argument check
    glUniform1i(glGetUniformLocation(shader->shader_program_id, name),value);
    // TODO: Error handling
}

void         set_shader_float     ( GXShader_t  *shader,       const char    *name,              float        value )
{
    // TODO: Argument check
    glUniform1f(glGetUniformLocation(shader->shader_program_id, name),value);
    // TODO: Error handling
}

void         set_shader_vec3      ( GXShader_t  *shader,       const char    *name,              vec3         vector )
{
    // TODO: Argument check
    glUniform3f(glGetUniformLocation(shader->shader_program_id, name), vector.x, vector.y, vector.z);
    // TODO: Error handling
}

void         set_shader_mat4      ( GXShader_t  *shader,       const char    *name,              mat4        *m )
{
    // TODO: Argument check
    glUniformMatrix4fv(glGetUniformLocation(shader->shader_program_id, name), 1, GL_FALSE, (const float*)m);
    // TODO: Error handling
}

void         set_shader_texture   ( GXShader_t  *shader,       const char    *name,              GXTexture_t *texture )
{
    // TODO: Argument check
    set_shader_int(shader, name, bind_texture_to_unit(texture));
    // TODO: Error handling
}
 
void         set_shader_transform ( GXShader_t  *shader,       GXTransform_t *transform )
{
    // TODO: Argument check
    GXUniform_t* i = shader->requested_data;

    // Update the model matrix
    make_model_matrix(&transform->model_matrix, transform);

    for (size_t j = 0; j < shader->requested_data_count && i; j++)
    {
        if (strcmp(i->key, "model") == 0)
            set_shader_mat4(shader, i->value, &transform->model_matrix);
        i = i->next;
    }
    // TODO: Error handlings
}

void         set_shader_camera    ( GXShader_t  *shader,       GXCamera_t    *camera )
{
    // TODO: Argument check
    GXUniform_t *i = shader->requested_data;

    for (size_t j = 0; j < shader->requested_data_count && i; j++)
    {
        if (strcmp(i->key,"camera position") == 0)
            set_shader_vec3(shader, i->value, camera->where);
        if (strcmp(i->key, "view") == 0) 
            set_shader_mat4(shader, i->value, &camera->view);
        if (strcmp(i->key, "projection") == 0)
            set_shader_mat4(shader, i->value, &camera->projection);
        i = i->next;
    }
    // TODO: Error handling
}

void         set_shader_lights    ( GXShader_t  *shader,       GXLight_t     *lights,            size_t       numLights )
{
    // TODO: Argument check
    GXUniform_t *i = shader->requested_data;
    GXLight_t   *l = lights;

    // Iterate through each uniform 
    for (size_t j = 0; j < shader->requested_data_count && i; j++)
    {
        // Check if the uniform is light position
        if (strcmp(i->key, "light position") == 0)
        {
            char  *value       = i->value;
            size_t valueLength = strlen(value);

            // Allocate for a string
            char *buffer = calloc(valueLength + 8 + 2 + 1, sizeof(u8));

            // Iterate through lights
            for (size_t k = 0; k < numLights && l; k++)
            {
                size_t nl = k;
                char  *p  = buffer + valueLength;

                // Set each shader uniform in the array
                strncpy(buffer, value, valueLength);

                *p++ = '[';
                *p++ = '0' + (nl % 10);
                nl /= 10;
                while (nl)
                {
                    *p++ = '0' + (nl % 10);
                    nl /= 10;
                }

                *p++ = ']';
                *p++ = '\0';

                set_shader_vec3(shader, buffer, l->location);
                l = l->next;
            }

            free(buffer);

            l = lights;
        }

        // Check if the uniform is light color
        if (strcmp(i->key, "light color") == 0)
        {
            char* value = i->value;
            size_t valueLength = strlen(value);

            // Allocate for a string
            char* buffer = calloc(valueLength + 8 + 2 + 1, sizeof(u8));

            // Iterate through lights
            for (size_t k = 0; k < numLights && l; k++)
            {
                size_t nl = k;
                char* p = buffer + valueLength;

                // Set each shader uniform in the array
                strncpy(buffer, value, valueLength);

                *p++ = '[';
                *p++ = '0' + (nl % 10);
                nl /= 10;
                while (nl)
                {
                    *p++ = '0' + (nl % 10);
                    nl /= 10;
                }

                *p++ = ']';
                *p++ = '\0';
                set_shader_vec3(shader, buffer, l->color);
                l = l->next;

            }

            free(buffer);
            
            l = lights;
        }

        i = i->next;
    }
    // TODO: Error handlings
}

void         set_shader_material  ( GXShader_t  *shader,       GXMaterial_t  *material ) 
{
    // TODO: Argument check
    GXUniform_t* i = shader->requested_data;

    for (size_t j = 0; j < shader->requested_data_count && i; j++)
    {
        if (strcmp(i->key, "albedo") == 0)
        {
            set_shader_texture(shader, i->value, material->albedo);
            goto ex;
        }
        if (strcmp(i->key, "specular") == 0)
        {
            set_shader_texture(shader, i->value, material->metal);
            goto ex;
        }
        if (strcmp(i->key, "normal") == 0)
        {
            set_shader_texture(shader, i->value, material->normal);
            goto ex;
        }
        if (strcmp(i->key, "metal") == 0)
        {
            set_shader_texture(shader, i->value, material->metal);
            goto ex;
        }
        if (strcmp(i->key, "rough") == 0)
        {
            set_shader_texture(shader, i->value, material->rough);
            goto ex;
        }
        if (strcmp(i->key, "ao") == 0)
        {
            set_shader_texture(shader, i->value, material->ao);
            goto ex;
        }

        ex:
        i = i->next;
        continue;
    }
    // TODO: Error handling
}

void         set_shader_ibl       ( GXShader_t  *shader,       GXSkybox_t    *skybox ) 
{
    // TODO: Argument check
    GXUniform_t* i = shader->requested_data;

    for (size_t j = 0; j < shader->requested_data_count && i; j++)
    {
        if (strcmp(i->key, "irradiance cubemap") == 0)
            set_shader_texture(shader, i->value, skybox->irradiance_cubemap);
        if (strcmp(i->key, "prefilter cubemap") == 0)
            set_shader_texture(shader, i->value, skybox->environment_cubemap);
        if (strcmp(i->key, "BRDF LUT") == 0)
            set_shader_texture(shader, i->value, skybox->lut);

        i = i->next;
    }
    // TODO: Error handling
}

void         set_shader_bone      ( GXShader_t  *shader,       const char    *uniformName,       GXBone_t* bone)
{
    // TODO: Argument check
    // Initialized data
    GXUniform_t *i              = shader->requested_data;
    GXBone_t    *lBone          = bone;

    size_t       uniformNameLen = strlen(uniformName);
    size_t       index          = bone->index;
    char        *p              = calloc(uniformNameLen+20,sizeof(u8)),
                *ret            = p;
    
    // Iterate through all bones
    while (lBone)
    {
        // Construct the uniform name and index
        sprintf(p, "%s[%d]", uniformName, index);

        set_shader_int(shader, ret, index);
        
        memset(ret, ' ', uniformNameLen + 20);

        if (lBone->children)
            set_shader_bone(shader, uniformName,  lBone->children);

        lBone = lBone->next;
    }

    free(ret);
    // TODO: Error handling
}

void         set_shader_rig       ( GXShader_t  *shader,        GXRig_t       *rig ) 
{
    // TODO: Argument check
    // Initialized data
    const char  *bonesUniformName = (void*)0;

    GXUniform_t *i                = shader->requested_data;

    // Iterate through each uniform
    for ( size_t j = 0; j < shader->requested_data_count && i; j++ )
    {
        
        // Is the uniform called bones? 
        if ( strcmp( i->key, "bones" ) == 0 )

            // Assign the bone uniform name.
            bonesUniformName = i->value;

        // Iterate
        i = i->next;
    }

    // If there is no name, thats a problem
    if ( bonesUniformName == 0 )
        goto noShaderRigSupport;

    set_shader_bone( shader, bonesUniformName, rig->bones );

    return 1;

    // Error handling
    {
        noShaderRigSupport:
        #ifndef NDEBUG
            g_print_error( "[G10] [Shader] Shader \"%s\" does not support bones\n", shader->name );
        #endif
        return 0;
    }

}

int          destroy_uniform      ( GXUniform_t *uniform )
{
    // Argument check
    {
        if (uniform == (void*)0)
            goto noUniform;
    }

    // Free up structure data
    free(uniform->key);
    free(uniform->value);

    // Free the structure itself
    free(uniform);
        
    return 0;

    // Error handling
    {
        noUniform:
        #ifndef NDEBUG
            g_print_error("[G10] [Shader] Null pointer provided for \"uniform\" in call to function \"%s\"\n",__FUNCSIG__);
        #endif      
        return 0;
    }
}

int          destroy_shader       ( GXShader_t  *shader )
{

    // Argument check
    {
        if (shader == (void*)0)
            goto noShader;
        if (shader->users > 1)
            goto in_use;
    }

    // Deallocate associated data
    GXUniform_t* i = shader->requested_data;

    while(i)
    {
        GXUniform_t *j = i;
        i = i->next;
        destroy_uniform(j);       

    }

    free(shader->name);

    glDeleteProgram(shader->shader_program_id);
    shader->shader_program_id = 0;

    free(shader);

    return 0;

    // Error handling
    {

        // Argument errors
        {
            noShader:
            #ifndef NDEBUG
                g_print_error("[G10] [Shader] Null pointer provided for \"shader\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif 
            return 0;
        }

        in_use:
            shader->users--;
            return 0;

    }
}

