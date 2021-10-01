#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/GXCamera.h>
#include <G10/GXTexture.h>
#include <G10/GXLinear.h>
#include <G10/GXShader.h>
#include <G10/GXHDR.h>

#define SKYBOX_RESOLUTION

struct GXSkybox_s
{
    GXPart_t    *cube;
    GXShader_t  *equirectangularToCubic,
                *irradiance,
                *background;
    GXTexture_t *HDR, 
                *environmentCubemap,
                *irradianceConvolution,
                *irradianceCubemap;
    mat4         perspective;
    mat4         views[6];
    u32          framebuffer,
                 renderbuffer;
};

GXSkybox_t *createSkybox     (  );                                        // ❌ Creates an empty skybox

GXSkybox_t *loadSkybox       ( const char  path[] );                      // ❌ Loads a skybox from a file
GXSkybox_t *loadSkyboxAsJSON ( char       *token );                       // ✅ Loads a skybox from a JSON token

int         drawSkybox       ( GXSkybox_t *skybox, GXCamera_t *camera );  // ✅ Draws the skybox 

int         destroySkybox    ( GXSkybox_t* skybox );                      // ✅ Destroys the skybox