#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <G10/GXDebug.h>
#include <G10/GXLinear.h>
#include <G10/GXEntity.h>

struct GXBVHNode_s
{
	GXEntity_t* entity;
	struct GXBVHNode_s* left;
	struct GXBVHNode_s* right;
};
typedef struct GXBVHNode_s GXBVHNode_t;

struct GXBVH_s
{
	GXBVHNode_t* root;
};
typedef struct GXBVH_s GXBVH_t;

GXBVH_t* createBVHTreeFromEntities(GXEntity_t* head);