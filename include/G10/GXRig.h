#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/GXLinear.h>
#include <G10/GXTransform.h>

struct GXBone_s
{
	char*               name;
	GXmat4_t*           transformation;
	struct GXBone_s*    parent;
	struct GXBone_s*    children;
	struct GXBone_s*    next;
};
typedef struct GXBone_s GXBone_t;

struct GXRig_s
{
	char*     name;
	GXBone_t* bones;
};
typedef struct GXRig_s GXRig_t;


GXBone_t* createBone  ( );
GXBone_t* findBone    ( GXRig_t* rig, char* name );
int       destroyBone ( GXBone_t* bone );

