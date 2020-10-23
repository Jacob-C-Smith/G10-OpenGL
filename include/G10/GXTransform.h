#pragma once

#include <stdlib.h>

#include <G10/GXLinear.h>
#include <G10/GXQuaternion.h>

struct GXTransform_s
{
	GXvec3_t location;
	GXvec3_t rotation;
	GXvec3_t scale;

	GXmat4_t modelMatrix;
};
typedef struct GXTransform_s GXtransform_t;

GXtransform_t* createTransform( GXvec3_t location, GXvec3_t rotation, GXvec3_t scale );
int            unloadTransform( GXtransform_t* transform );