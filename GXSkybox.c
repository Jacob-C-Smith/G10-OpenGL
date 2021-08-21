#include <G10/GXSkybox.h>


GXSkybox_t* createSkybox()
{
	GXSkybox_t* ret = calloc(1, sizeof(GXSkybox_t));

	return ret;
}

GXSkybox_t* loadSkybox(const char path[])
{
	GXSkybox_t *ret             = createSkybox();

	ret->cube                   = loadPart("G10/cube.json");
	
	ret->equirectangularToCubic = loadShader("G10/equirectangular to cubemap.json");
	ret->background             = loadShader("G10/background.json");
	
	ret->HDR                    = loadHDR(path);
	ret->environmentCubemap     = createTexture();

	ret->perspective            = perspective(M_PI_2, 1.f, 0.1f, 10.f);

	ret->views[0] = lookAt( (GXvec3_t) { 0.f, 0.f, 0.f }, (GXvec3_t) { -1.f, 0.f , 0.f  }, (GXvec3_t) { 0.f, 0.f , 1.f } ); // +X 
	ret->views[1] = lookAt( (GXvec3_t) { 0.f, 0.f, 0.f }, (GXvec3_t) { 1.f , 0.f , 0.f  }, (GXvec3_t) { 0.f, 0.f , 1.f } ); // -X 
	ret->views[2] = lookAt( (GXvec3_t) { 0.f, 0.f, 0.f }, (GXvec3_t) { 0.f , 0.f , -1.f }, (GXvec3_t) { 0.f, -1.f, 0.f } ); // +Y
	ret->views[3] = lookAt( (GXvec3_t) { 0.f, 0.f, 0.f }, (GXvec3_t) { 0.f , 0.f , 1.f  }, (GXvec3_t) { 0.f, 1.f , 0.f } ); // -Y
	ret->views[4] = lookAt( (GXvec3_t) { 0.f, 0.f, 0.f }, (GXvec3_t) { 0.f , -1.f, 0.f  }, (GXvec3_t) { 0.f, 0.f , 1.f } ); // +Z
	ret->views[5] = lookAt( (GXvec3_t) { 0.f, 0.f, 0.f }, (GXvec3_t) { 0.f , 1.f , 0.f  }, (GXvec3_t) { 0.f, 0.f , 1.f } ); // -Z

	// Create a framebuffer and a renderbuffer
	glGenFramebuffers(1, &ret->framebuffer);
	glGenRenderbuffers(1, &ret->renderbuffer);

	// Bind the framebuffer and the renderbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, ret->framebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, ret->renderbuffer);

	// Render a 1024x1024
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ret->renderbuffer);

	glGenTextures(1, &ret->environmentCubemap->textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ret->environmentCubemap->textureID);
	
	for (u32 i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, (void*)0);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	useShader(ret->equirectangularToCubic);
	setShaderInt(ret->equirectangularToCubic, "equirectangularMap", loadTextureToTextureUnit(ret->HDR));
	setShaderMat4(ret->equirectangularToCubic, "P", &ret->perspective);

	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, ret->framebuffer);

	for (u32 i = 0; i < 6; ++i)
	{
		setShaderMat4(ret->equirectangularToCubic, "V", &ret->views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ret->environmentCubemap->textureID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawPart(ret->cube);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return ret;
}

GXSkybox_t* loadSkyboxAsJSON(char* token)
{
	// Uninitialized data
    i32          tokenCount;
    JSONValue_t *tokens;

    // Initialized data
	size_t       i   = strlen(token);
    GXSkybox_t  *ret = createSkybox();
    size_t       l   = 0;

    // Preparse the skybox
    l                = strlen(token);
    tokenCount       = GXParseJSON(token, i, 0, (void*)0);
    tokens           = calloc(tokenCount, sizeof(JSONValue_t));

    // Parse the skybox
    GXParseJSON(token, i, tokenCount, tokens);
	
    // Find and exfiltrate pertinent information.
    for (size_t j = 0; j < tokenCount; j++)
    {
        // Set up lights
        if (strcmp("equirectangular", tokens[j].name) == 0)
        {
			ret->HDR = loadHDR(tokens[j].content.nWhere);
			continue;
        }

    }

	ret->cube = loadPart("G10/cube.json");

	ret->equirectangularToCubic = loadShader("G10/equirectangular to cubemap.json");
	ret->background = loadShader("G10/background.json");

	ret->environmentCubemap = createTexture();

	ret->perspective = perspective(M_PI_2, 1.f, 0.1f, 10.f);

	ret->views[0] = lookAt((GXvec3_t) { 0.f, 0.f, 0.f }, (GXvec3_t) { -1.f, 0.f, 0.f }, (GXvec3_t) { 0.f, 0.f, 1.f }); // +X 
	ret->views[1] = lookAt((GXvec3_t) { 0.f, 0.f, 0.f }, (GXvec3_t) { 1.f, 0.f, 0.f }, (GXvec3_t) { 0.f, 0.f, 1.f }); // -X 
	ret->views[2] = lookAt((GXvec3_t) { 0.f, 0.f, 0.f }, (GXvec3_t) { 0.f, 0.f, -1.f }, (GXvec3_t) { 0.f, -1.f, 0.f }); // +Y
	ret->views[3] = lookAt((GXvec3_t) { 0.f, 0.f, 0.f }, (GXvec3_t) { 0.f, 0.f, 1.f }, (GXvec3_t) { 0.f, 1.f, 0.f }); // -Y
	ret->views[4] = lookAt((GXvec3_t) { 0.f, 0.f, 0.f }, (GXvec3_t) { 0.f, -1.f, 0.f }, (GXvec3_t) { 0.f, 0.f, 1.f }); // +Z
	ret->views[5] = lookAt((GXvec3_t) { 0.f, 0.f, 0.f }, (GXvec3_t) { 0.f, 1.f, 0.f }, (GXvec3_t) { 0.f, 0.f, 1.f }); // -Z

	// Create a framebuffer and a renderbuffer
	glGenFramebuffers(1, &ret->framebuffer);
	glGenRenderbuffers(1, &ret->renderbuffer);

	// Bind the framebuffer and the renderbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, ret->framebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, ret->renderbuffer);

	// Render a 1024x1024
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ret->renderbuffer);

	glGenTextures(1, &ret->environmentCubemap->textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ret->environmentCubemap->textureID);

	for (u32 i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, (void*)0);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	useShader(ret->equirectangularToCubic);
	setShaderInt(ret->equirectangularToCubic, "equirectangularMap", loadTextureToTextureUnit(ret->HDR));
	setShaderMat4(ret->equirectangularToCubic, "P", &ret->perspective);

	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, ret->framebuffer);

	for (u32 i = 0; i < 6; ++i)
	{
		setShaderMat4(ret->equirectangularToCubic, "V", &ret->views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ret->environmentCubemap->textureID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawPart(ret->cube);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Finish up
    free(tokens);
    return ret;
}



int drawSkybox(GXSkybox_t *skybox, GXCamera_t *camera)
{

	useShader(skybox->background);
	setShaderMat4(skybox->background, "P", &camera->projection);
	setShaderMat4(skybox->background, "V", &camera->view);
	skybox->environmentCubemap->cubemap = 1;
	setShaderInt(skybox->background, "environmentMap", loadTextureToTextureUnit(skybox->environmentCubemap));
	drawPart(skybox->cube);

	return 0;
}

int destroySkybox(GXSkybox_t* skybox)
{
	return 0;
}
