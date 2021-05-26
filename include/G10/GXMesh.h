#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>

// Stores information about a mesh
struct GXMesh_s
{
	// Name
	char*     name;

	// Array
	GLuint    vertexArray;

	// Geometric vertecies
	GLuint    vertexBuffer;
	GLuint    elementBuffer;

	GLuint    elementsInBuffer;
};
typedef struct GXMesh_s GXMesh_t;

// Constructors
GXMesh_t* createMesh       ( );                   // ✅ Creates an empty mesh
GXMesh_t* loadMesh         ( const char path[] ); // ✅ Loads a mesh from the disk
GXMesh_t* loadMeshFromJSON ( const char path[] ); // ❌ Loads a mesh from a JSON file

// Drawing protocols
int       drawMesh         ( GXMesh_t* mesh );    // ❌ Draws a mesh with the current shader program

// Destructors
int       unloadMesh       ( GXMesh_t* mesh );    // ✅ Destroy a mesh and all of its contents
