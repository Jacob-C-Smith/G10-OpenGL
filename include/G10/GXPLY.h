#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/GXtypedef.h>
#include <G10/GXMesh.h>

#define GXPLY_Geometric 0x1
#define GXPLY_Texture   0x2
#define GXPLY_Normal    0x4
#define GXPLY_Color     0x8

struct GXPLYproperty_s
{
	size_t typeSize;
	char* name;
};
typedef struct GXPLYproperty_s GXPLYproperty_t;

struct GXPLYelement_s
{
	size_t           nCount;
	size_t           nProperties;
	size_t           sStride;
	GXPLYproperty_t* properties;
	char*            name;
};
typedef struct GXPLYelement_s GXPLYelement_t;

struct GXPLYfile_s {
	size_t        nElements;
	GXPLYelement_t* elements;
	size_t        flags;
};
typedef struct GXPLYfile_s GXPLYfile_t;

// The indicies come in packed data structures. In order for the compiler and thus G10 to understand them, a custom struct has to be defined that fits the data structure offsets.
#pragma pack (push)
#pragma pack (1) // Sorry RISC platforms :(
struct GXPLYindex_s {
	u8 count;
	u32 a;
	u32 b;
	u32 c;
};
#pragma pack (pop)
typedef struct GXPLYindex_s GXPLYindex_t;

GXMesh_t* loadPLYMesh ( const char path[] ); // ✅ Loads a PLY file from path and returns a pointer to GXMesh_t
