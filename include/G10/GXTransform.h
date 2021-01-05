#pragma once

#include <stdlib.h>

#include <G10/GXLinear.h>
#include <G10/GXQuaternion.h>

// Contains information about a transform
struct GXTransform_s
{
	GXvec3_t location;
	GXvec3_t rotation;
	GXvec3_t scale;

	GXmat4_t modelMatrix;
};
typedef struct GXTransform_s GXtransform_t;

GXtransform_t* createTransform( GXvec3_t location, GXvec3_t rotation, GXvec3_t scale ); // ✅ Creates a transform from location, rotation, and scale
int            unloadTransform( GXtransform_t* transform );                             // ✅ Destroys a transform