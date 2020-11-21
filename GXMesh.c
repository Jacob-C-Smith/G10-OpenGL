#include <G10/GXMesh.h>

int unloadMesh( GXmesh_t* mesh )
{
	// Invalidate v, vt, vn, and f counts
	mesh->geometricVerticesCount = 0;
	mesh->textureCoordinatesCount = 0;
	mesh->vertexNormalsCount = 0;
	mesh->facesCount = 0;

	// Free v, vt, vn
	free(mesh->faces.v);
	free(mesh->faces.vt);
	free(mesh->faces.vn);

	// Invalidate v, vt, and vn pointers
	mesh->faces.v  = (void*)0;
	mesh->faces.vt = (void*)0;
	mesh->faces.vn = (void*)0;

	// Free up OpenGL buffers
	glDeleteVertexArrays(1, &mesh->vertexArray);
	glDeleteBuffers(1, &mesh->vertexBuffer);
	glDeleteBuffers(1, &mesh->elementBuffer);
	glDeleteBuffers(1, &mesh->textureBuffer);

	// Invalidate OpenGL buffers
	mesh->vertexArray   = 0;
	mesh->vertexBuffer  = 0;
	mesh->elementBuffer = 0;
	mesh->textureBuffer = 0;

	// Invalidate mesh
	free(mesh);

	// Return nullptr
	return 0;
}

