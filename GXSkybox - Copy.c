#include <G10/GXSkybox.h>


GXSkybox_t *createSkybox     ( )
{
    GXSkybox_t* ret = calloc(1, sizeof(GXSkybox_t));
    
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
            gPrintError("[G10] [Skybox] Out of memory.\n");
        #endif
        return 0;
    }
}

GXSkybox_t *loadSkybox       ( const char  path[] )
{
    // Argument check
    {
        #ifndef NDEBUG
        if (path == (void*)0)
            goto noPath;
        #endif
    }

    // Uninitialized data
    char        *data;
    size_t       i;

    // Initialized data
    GXSkybox_t *ret = (void*)0;

    // Load up the file
    i    = gLoadFile(path, 0, false);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data, false);

    // Parse the JSON
    ret  = loadSkyboxAsJSON(data);
    
    // Clean up the memory
    free(data);

    // Return
    return ret;

    // Error handling
    {
        noPath:
        #ifndef NDEBUG
            gPrintError("[G10] [Skybox] No path supplied to function \"%s\"\n",__FUNCSIG__);
        #endif
        return (void*)0;
    }
}

GXSkybox_t* loadSkyboxAsJSON ( char *token )
{
    // TODO: Argument check

    // Uninitialized data

    // Initialized data
    GXSkybox_t  *ret        = createSkybox();
    char        *envPath    = 0,
                *irrPath    = 0;
    size_t       len        = strlen(token),
                 tokenCount = GXParseJSON(token, len, 0, (void*)0);
    JSONValue_t *tokens     = calloc(tokenCount, sizeof(JSONValue_t));

    GXTexture_t* environmentHDR;

    u32         envFramebuffer   = -1,
                envRenderbuffer  = -1,
                irrFramebuffer   = -1,
                irrRenderbuffer  = -1,
                preFramebuffer   = -1,
                preRenderbuffer  = -1,
                LUTFramebuffer   = -1,
                LUTRenderbuffer  = -1;


    // Parse the skybox token
    GXParseJSON(token, len, tokenCount, tokens);

    // Search through values and pull out relevent information
    for (size_t j = 0; j < tokenCount; j++)
    {
        // Name
        if (strcmp("name", tokens[j].name) == 0)
        {
            size_t len = strlen(tokens[j].name);
            ret->name  = calloc(len+1, sizeof(u8));
            
            strncpy(ret->name, tokens[j].content.nWhere, len);
        }

        // Environment
        if (strcmp("environment", tokens[j].name) == 0)
        {
            envPath = tokens[j].content.nWhere;
        }

        // Irradiance
        if (strcmp("irradiance", tokens[j].name) == 0)
        {
            irrPath = tokens[j].content.nWhere;
        }

        // Color
        if (strcmp("color", tokens[j].name) == 0)
        {
            // TODO: Set clear color to color, if there are no cubemap textures
        }
    }

    ret->environmentCubemap          = createTexture();
    ret->irradianceCubemap           = createTexture();
    ret->prefilterCubemap            = createTexture();
    ret->LUT                         = createTexture();

    ret->environmentCubemap->cubemap = true;
    ret->irradianceCubemap->cubemap  = true;
    ret->prefilterCubemap->cubemap   = true;
    
    // Generate perspective and view matrices

    // 90 degree fov, 1:1 aspect ratio
    ret->projection = perspective(M_PI_2, 1.f, .1f, 10.f);

    ret->views[0] = lookAt((vec3) { 0.f, 0.f, 0.f }, (vec3) { -1.f, 0.f, 0.f }, (vec3) { 0.f, 0.f, 1.f });  //  -X 
    ret->views[1] = lookAt((vec3) { 0.f, 0.f, 0.f }, (vec3) { 1.f, 0.f, 0.f }, (vec3) { 0.f,  0.f, 1.f });  //  +X 
    ret->views[2] = lookAt((vec3) { 0.f, 0.f, 0.f }, (vec3) { 0.f, 0.f, -1.f }, (vec3) { 0.f, -1.f, 0.f }); //  -Z
    ret->views[3] = lookAt((vec3) { 0.f, 0.f, 0.f }, (vec3) { 0.f, 0.f,  1.f }, (vec3) { 0.f, 1.f, 0.f });  //  +Z
    ret->views[4] = lookAt((vec3) { 0.f, 0.f, 0.f }, (vec3) { 0.f, -1.f, 0.f }, (vec3) { 0.f, 0.f, 1.f });  //  -Y
    ret->views[5] = lookAt((vec3) { 0.f, 0.f, 0.f }, (vec3) { 0.f, 1.f, 0.f }, (vec3) { 0.f, 0.f, 1.f });   //  +Y


    // Load the required shaders

    // Turns an equirectangular image into a cubemap
    ret->environment = loadShader("G10/shaders/G10 environment.json");
    ret->irradiance  = loadShader("G10/shaders/G10 irradiance.json");
    ret->prefilter   = loadShader("G10/shaders/G10 prefilter.json");
    ret->brdf        = loadShader("G10/shaders/G10 BRDF.json");

    // Draws the skybox
    ret->background  = loadShader("G10/shaders/G10 background.json");

    // Load the cube part
    ret->cube        = loadPart("G10/cube.json");
    ret->quad        = loadPart("G10/plane.json");

    // Load the environment maps and the irradiance convolution
    environmentHDR = loadHDR(envPath);

    glDepthFunc(GL_LEQUAL);

    // Generate environment cubemap
    {
        // Generate environment framebuffers
        glGenFramebuffers(1, &envFramebuffer);
        glGenRenderbuffers(1, &envRenderbuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, envFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, envRenderbuffer);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, envRenderbuffer);

        glGenTextures(1, &ret->environmentCubemap->textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ret->environmentCubemap->textureID);

        // Generate cubemap textures
        for (u32 i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, (void*)0);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        useShader(ret->environment);
        setShaderTexture(ret->environment, "equirectangularMap", environmentHDR);
        setShaderMat4(ret->environment, "P", &ret->projection);

        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, envFramebuffer);

        for (u32 i = 0; i < 6; ++i)
        {
            setShaderMat4(ret->environment, "V", &ret->views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ret->environmentCubemap->textureID, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // If this stops working, be sure that face culling is off.

            drawPart(ret->cube);
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, ret->environmentCubemap->textureID);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDeleteRenderbuffers(1, &envRenderbuffer);
        glDeleteFramebuffers(1, &envFramebuffer);
    }
    
    // Generate irradiance cubemap
    {
        // Generate irradiance framebuffers
        glGenFramebuffers(1, &irrFramebuffer);
        glGenRenderbuffers(1, &irrRenderbuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, irrFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, irrRenderbuffer);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, irrRenderbuffer);

        glGenTextures(1, &ret->irradianceCubemap->textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ret->irradianceCubemap->textureID);

        glBindFramebuffer(GL_FRAMEBUFFER, irrFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, irrRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

        for (u32 i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, (void*)0);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        useShader(ret->irradiance);
        setShaderMat4(ret->irradiance, "P", &ret->projection);
        setShaderTexture(ret->irradiance, "environmentMap", ret->environmentCubemap);
        
        glViewport(0, 0, 32, 32);
        glBindFramebuffer(GL_FRAMEBUFFER, irrFramebuffer);
        for (u32 i = 0; i < 6; ++i)
        {
            setShaderMat4(ret->irradiance, "V", &ret->views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ret->irradianceCubemap->textureID, 0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            drawPart(ret->cube);
        }
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDeleteRenderbuffers(1, &irrRenderbuffer);
        glDeleteFramebuffers(1, &irrFramebuffer);
    }

    // Create a 2D lookup table from the BRDF equations
    {
        glGenTextures(1, &ret->LUT->textureID);

        glBindTexture(GL_TEXTURE_2D, ret->LUT->textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    return ret;

    // TODO: Error handling
}

int         drawSkybox       ( GXSkybox_t *skybox, GXCamera_t *camera )
{
    GXShader_t *shader = skybox->background;

    useShader(shader);
    setShaderCamera(shader, camera);
    setShaderTexture(shader, "environmentMap", skybox->environmentCubemap);
    drawPart(skybox->cube);

    return 0;
}

int         destroySkybox    ( GXSkybox_t *skybox )
{
    return 0;
}
