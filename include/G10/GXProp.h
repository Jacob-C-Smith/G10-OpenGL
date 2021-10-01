#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/GXTexture.h>
#include <G10/GXMesh.h>

// Props are kind of like diet entities. They are fixed, do not compute
struct GXProp_s {
	char        *name;
	GXPart_t    *part;
	GXTexture_t *texture;
};

GXProp_t* createProp     ( );

GXProp_t* loadProp       ( const char path[] );
GXProp_t* loadPropAsJSON ( char      *token );

int       drawProp       ( GXProp_t  *prop );

int       destroyProp    ( GXProp_t  *prop );
