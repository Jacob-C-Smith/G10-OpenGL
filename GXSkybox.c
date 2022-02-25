#include <G10/GXSkybox.h>


GXSkybox_t *create_skybox     ( )
{
    GXSkybox_t* ret = calloc(1, sizeof(GXSkybox_t));
    
    // Check the memory
    {
        #ifndef NDEBUG
            if (ret == 0)
                goto no_mem;
        #endif
    }

    return ret;

    // Error handling
    {
        no_mem:
        #ifndef NDEBUG
            g_print_error("[G10] [Skybox] Out of memory.\n");
        #endif
        return 0;
    }
}

GXSkybox_t *load_skybox       ( const char  path[] )
{

    // Argument check
    {
        #ifndef NDEBUG
            if (path == (void*)0)
                goto no_path;
        #endif
    }

    // Uninitialized data
    char        *data;
    size_t       i;

    // Initialized data
    GXSkybox_t *ret = (void*)0;

    // Load up the file
    i    = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    // Parse the JSON
    ret  = load_skybox_as_json(data);
    
    // Clean up the memory
    free(data);

    // Return
    return ret;

    // Error handling
    {
        no_path:
        #ifndef NDEBUG
            g_print_error("[G10] [Skybox] Null pointer provided for \"path\" in call to \"%s\"\n",__FUNCSIG__);
        #endif
        return (void*)0;
    }
}

GXSkybox_t* load_skybox_as_json ( char *token )
{
    // Argument check
    {
        #ifndef NDEBUG
            if(token==(void*)0)
                goto no_token;
        #endif
    }   

    // Uninitialized data

    // Initialized data
    GXSkybox_t  *ret        = create_skybox();
    char        *envPath    = 0,
                *irrPath    = 0;
    size_t       len        = strlen(token),
                 token_count = parse_json(token, len, 0, (void*)0);
    JSONToken_t *tokens     = calloc(token_count, sizeof(JSONToken_t));

    GXTexture_t* environmentHDR;

    u32         framebuffer   = -1,
                renderbuffer  = -1;

    // Parse the skybox token
    parse_json(token, len, token_count, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < token_count; j++)
    {
        // Name
        if (strcmp("name", tokens[j].key) == 0)
        {
            size_t len = strlen(tokens[j].key);
            ret->name  = calloc(len+1, sizeof(u8));
            
            strncpy(ret->name, tokens[j].value.n_where, len);
        }

        // Environment
        if (strcmp("environment", tokens[j].key) == 0)
        {
            envPath = tokens[j].value.n_where;
        }

        // Irradiance
        if (strcmp("irradiance", tokens[j].key) == 0)
        {
            irrPath = tokens[j].value.n_where;
        }

        // Color
        if (strcmp("color", tokens[j].key) == 0)
        {
            // TODO: Set clear color to color, if there are no cubemap textures
        }
    }

    ret->environment_cubemap          = create_texture();
    ret->irradiance_cubemap           = create_texture();
    ret->prefilter_cubemap            = create_texture();
    
    // Generate perspective and view matrices

    // 90 degree fov, 1:1 aspect ratio
    ret->projection = perspective(M_PI_2, 1.f, .1f, 10.f);

    ret->views[0] = look_at((vec3) { 0.f, 0.f, 0.f }, (vec3) { -1.f, 0.f, 0.f }, (vec3) { 0.f, 0.f, 1.f });  //  -X 
    ret->views[1] = look_at((vec3) { 0.f, 0.f, 0.f }, (vec3) { 1.f, 0.f, 0.f }, (vec3) { 0.f,  0.f, 1.f });  //  +X 
    ret->views[2] = look_at((vec3) { 0.f, 0.f, 0.f }, (vec3) { 0.f, 0.f, -1.f }, (vec3) { 0.f, -1.f, 0.f }); //  -Z
    ret->views[3] = look_at((vec3) { 0.f, 0.f, 0.f }, (vec3) { 0.f, 0.f,  1.f }, (vec3) { 0.f, 1.f, 0.f });  //  +Z
    ret->views[4] = look_at((vec3) { 0.f, 0.f, 0.f }, (vec3) { 0.f, -1.f, 0.f }, (vec3) { 0.f, 0.f, 1.f });  //  -Y
    ret->views[5] = look_at((vec3) { 0.f, 0.f, 0.f }, (vec3) { 0.f, 1.f, 0.f }, (vec3) { 0.f, 0.f, 1.f });   //  +Y


    // Load the required shaders

    // Turns an equirectangular image into a cubemap
    ret->environment = load_shader("G10/shaders/G10 environment.json");
    ret->irradiance  = load_shader("G10/shaders/G10 irradiance.json");
    ret->prefilter   = load_shader("G10/shaders/G10 prefilter.json");
    ret->brdf        = load_shader("G10/shaders/G10 BRDF.json");

    // Draws the skybox
    ret->background  = load_shader("G10/shaders/G10 background.json");

    // Load the cube part
    ret->cube        = load_part("G10/cube.json");
    ret->quad        = load_part("G10/plane.json");
    ret->lut         = load_hdr("G10/BRDF.hdr");


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Generate environment cubemap
    {
        // Generate environment framebuffers
        glGenFramebuffers(1, &framebuffer);
        glGenRenderbuffers(1, &renderbuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

        // Load the environment maps and the irradiance convolution
        environmentHDR = load_hdr(envPath);


        glGenTextures(1, &ret->environment_cubemap->texture_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ret->environment_cubemap->texture_id);
        ret->environment_cubemap->cubemap = true;

        // Generate cubemap textures
        for (u32 i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, (void*)0);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        use_shader(ret->environment);
        set_shader_texture(ret->environment, "equirectangularMap", environmentHDR);
        set_shader_mat4(ret->environment, "P", &ret->projection);

        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        for (u32 i = 0; i < 6; ++i)
        {
            set_shader_mat4(ret->environment, "V", &ret->views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ret->environment_cubemap->texture_id, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            draw_part(ret->cube);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, ret->environment_cubemap->texture_id);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    
    // Generate the irradiance cubemap
    {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteRenderbuffers(1, &renderbuffer);

        // Generate environment framebuffers
        glGenFramebuffers(1, &framebuffer);
        glGenRenderbuffers(1, &renderbuffer);

        glGenTextures(1, &ret->irradiance_cubemap->texture_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ret->irradiance_cubemap->texture_id);
        ret->irradiance_cubemap->cubemap = true;

        for(u32 i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, (void*)0);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

        use_shader(ret->irradiance);
        set_shader_texture(ret->irradiance, "environmentMap", ret->environment_cubemap);
        set_shader_mat4(ret->irradiance, "P", &ret->projection);

        glViewport(0, 0, 32, 32);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        for (u32 i = 0; i < 6; ++i)
        {
            set_shader_mat4(ret->irradiance, "V", &ret->views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ret->irradiance_cubemap->texture_id, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            draw_part(ret->cube);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, ret->environment_cubemap->texture_id);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    
    // Generate the prefilter map
    {
           
    }

    return ret;

    // Error handling
    {
        #ifndef NDEBUG
        no_token:
            g_print_error("[G10] [Skybox] Null pointer provided for \"token\" in call to function \"%s\"",__FUNCSIG__);
        #endif
        return 0;
    }
}

int         draw_skybox       ( GXSkybox_t *skybox )
{

    // Argument check
    {
        if(skybox == (void *)0)
            goto no_skybox;
    }

    GXInstance_t *instance = g_get_active_instance();
    GXShader_t   *shader   = skybox->background;

    use_shader(shader);
    set_shader_texture(shader, "environmentMap", skybox->environment_cubemap);
    set_shader_camera(shader, instance->scenes->cameras);
    draw_part(skybox->cube);

    return 0;

    // Error handling
    {

        // Argument check
        {
            no_skybox:
            #ifndef NDEBUG
                g_print_error("[G10] [Skybox] Null pointer provided for \"skybox\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif  
            return 0;
        }
    }
}

// TODO: Write
int         destroy_skybox    ( GXSkybox_t *skybox )
{
    // Argument check
    {
        if(skybox == (void *)0)
            goto no_skybox;
    }

    return 0;

    // Error handling
    {

        // Argument check
        {
            no_skybox:
            #ifndef NDEBUG
                g_print_error("[G10] [Skybox] Null pointer provided for \"skybox\" in call to function \"%s\"\n",__FUNCSIG__);
            #endif
            return 0;
        }
    }

}
