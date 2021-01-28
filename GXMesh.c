#include <G10/GXMesh.h>

#include <G10/GXPLY.h>

GXMesh_t* createMesh ( )
{
	// Initialized data
	GXMesh_t* ret = malloc(sizeof(GXMesh_t));
	// Check allocated memory
	if (ret == 0)
		return ret;
	// Set vertex arrays, vertex buffers, and element buffers to 0
	ret->vertexArray      = 0;
	ret->vertexBuffer     = 0;
	ret->elementBuffer    = 0;
	ret->elementsInBuffer = 0;

	return ret;
}

GXMesh_t* loadMesh ( const char path[] )
{
	// Initialized data
	char*     fileExtension = 1 + strrchr(path, '.');
	GXMesh_t* ret           = (void*)0;

	// Figure out what type of file we are dealing with throught the extenstion. This is an admittedly naive approach, but each loader function checks for signatures, so any error handling is handed off to them
	if (strcmp(fileExtension, "ply") == 0 || strcmp(fileExtension, "PLY") == 0)
		ret = loadPLYMesh(path);
	else
	#if GXDEBUGMODE & GXDEBUGMESH
		printf("Could not load file %s\n", path);
	#endif

	return ret;
}

int unloadMesh ( GXMesh_t* mesh )
{
	// Free up OpenGL buffers
	glDeleteVertexArrays(1, &mesh->vertexArray);
	glDeleteBuffers(1, &mesh->vertexBuffer);
	glDeleteBuffers(1, &mesh->elementBuffer);

	// Invalidate OpenGL buffers
	mesh->vertexArray      = 0;
	mesh->vertexBuffer     = 0;
	mesh->elementBuffer    = 0;
	mesh->elementsInBuffer = 0;

	// Invalidate mesh
	free(mesh);

	return 0;
}

