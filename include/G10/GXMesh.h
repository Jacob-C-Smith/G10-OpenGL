#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXDebug.h>
#include <G10/GXtypedef.h>
#include <G10/GXDimension.h>

// Stores information about faces
struct faces_s
{
	int3_t* v;  // Indicies to draw face from
	int3_t* vt; // Indicies to map textures to
	int3_t* vn; // Indicies to map normals to
};
typedef struct faces_s faces_t;

// Stores information about a mesh
struct GXMesh_s
{
	// Faces for Element buffer 
	faces_t   faces;

	// Counts for different coordinates
	GXsize_t  geometricVerticesCount;
	GXsize_t  textureCoordinatesCount;
	GXsize_t  vertexNormalsCount;
	GXsize_t  facesCount;

	// Array
	GLuint    vertexArray;

	// geometric vertecies
	GLuint    vertexBuffer;
	GLuint    elementBuffer;

	// texture
	GLuint    textureBuffer;

	// normals
	GLuint    normalBuffer;
};
typedef struct GXMesh_s GXmesh_t;

int unloadMesh ( GXmesh_t* mesh ); // ✅ Destroy a mesh and all of its contents
