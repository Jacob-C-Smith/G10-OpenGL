#pragma once

#include <stdlib.h>

#include <G10/GXLinear.h>
#include <G10/GXQuaternion.h>

// Contains information about a transform

#pragma pack (push)
#pragma pack (0x10) // Sorry RISC platforms :(
struct GXTransform_s
{
	GXvec3_t location;
	GXvec3_t rotation;
	GXvec3_t scale;

	GXmat4_t modelMatrix;
};
#pragma pack (pop)
typedef struct GXTransform_s GXTransform_t;

// Constructors
GXTransform_t* createTransform ( GXvec3_t location, GXvec3_t rotation, GXvec3_t scale ); // ✅ Creates a transform from location, rotation, and scale

// Loaders
GXTransform_t* loadTransform   ( const char path[] );

// Destructors
int            unloadTransform ( GXTransform_t* transform );                             // ✅ Destroys a transform