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
    GXShader_t  *equirectangularToCubic;
    GXShader_t  *background;
    GXTexture_t *HDR, 
                *environmentCubemap;
    GXmat4_t     perspective;
    GXmat4_t     views[6];
    u32          framebuffer,
                 renderbuffer;
};
typedef struct GXSkybox_s GXSkybox_t;

GXSkybox_t *createSkybox     (  );

GXSkybox_t *loadSkybox       ( const char  path[] );
GXSkybox_t *loadSkyboxAsJSON ( char       *token );

int         drawSkybox       ( GXSkybox_t *skybox, GXCamera_t *camera );

int         destroySkybox    ( GXSkybox_t* skybox );