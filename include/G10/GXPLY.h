#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/GXdebug.h>
#include <G10/GXtypedef.h>
#include <G10/GXMesh.h>

#define GXPLY_Geometric 0x1
#define GXPLY_Texture   0x2
#define GXPLY_Normal    0x4
#define GXPLY_Color     0x8

struct GXPLYlist_s {
	size_t a;
};
typedef struct GXPLYlist_s GXPLYlist_t;


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

	char* name;
};
typedef struct GXPLYelement_s GXPLYelement_t;

struct GXPLYfile_s {
	GXsize_t        nElements;
	GXPLYelement_t* elements;
	GXsize_t        flags;
};
typedef struct GXPLYfile_s GXPLYfile_t;

GXmesh_t* loadPLYMesh ( const char path[] ); // ✅ Loads an OBJ file from path and returns a pointer to GXmesh_t
