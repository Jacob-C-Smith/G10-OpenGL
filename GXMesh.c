#include <G10/GXMesh.h>

int unloadMesh(GXmesh_t* mesh)
{
	// Invalidate v, vt, vn, and f counts
	mesh->geometricVerticesCount = 0;
	mesh->textureCoordinatesCount = 0;
	mesh->vertexNormalsCount = 0;
	mesh->facesCount = 0;

	// Free v, vt, vn
	// Free faces
	free(mesh->faces.v);
	free(mesh->faces.vt);
	free(mesh->faces.vn);

	mesh->faces.v  = (void*)0;
	mesh->faces.vt = (void*)0;
	mesh->faces.vn = (void*)0;

	// Invalidate mesh
	free(mesh);

	return 0;
}

