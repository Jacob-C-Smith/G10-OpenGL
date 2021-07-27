#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/GXCamera.h>
#include <G10/GXTexture.h>
#include <G10/GXLinear.h>
#include <G10/GXShader.h>
#include <G10/GXPNG.h>

#define SKYBOX_RESOLUTION

struct GXSkybox_s
{
    GXTexture_t *texture;
    GXmat4_t    *captureViews,
                 captureProjection;
    GXShader_t  *skyboxShader;
    unsigned int framebuffer,
                 renderbuffer,
                 cubemap,
                 vArray,
                 vBuffer,
                 eBuffer;
};
typedef struct GXSkybox_s GXSkybox_t;

GXSkybox_t *createSkybox ( const char  path[] );
int         drawSkybox   ( GXSkybox_t *skybox );
