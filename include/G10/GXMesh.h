#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXDebug.h>
#include <G10/GXtypedef.h>
#include <G10/GXDimension.h>

// Stores information about a mesh
struct GXMesh_s
{
	// Array
	GLuint    vertexArray;

	// Geometric vertecies
	GLuint    vertexBuffer;
	GLuint    elementBuffer;

	GLuint    elementsInBuffer;
};
typedef struct GXMesh_s GXMesh_t;

GXMesh_t* createMesh ( );                   // ✅ Creates an empty mesh
GXMesh_t* loadMesh   ( const char path[] ); // ✅ Loads a mesh from the disk
int       unloadMesh ( GXMesh_t* mesh );    // ✅ Destroy a mesh and all of its contents
