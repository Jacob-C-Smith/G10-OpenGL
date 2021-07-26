#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>
#include <G10/GXJSON.h>
#include <G10/GXMaterial.h>

// Stores information about a part of a mesh
struct GXPart_s
{
    // Name
    char               *name;

    // Material
    GXMaterial_t       *material;

    // Vertex groups
    size_t              vertexGroups;

    // Array
    GLuint              vertexArray;

    // Geometric vertecies
    GLuint              vertexBuffer;
    GLuint              elementBuffer;

    GLuint              elementsInBuffer;

    struct GXPart_s    *next;
};
typedef struct GXPart_s GXPart_t;

// Stores information about a mesh
struct GXMesh_s
{
    // Name
    char        *name;

    // Parts of the mesh
    GXPart_t    *parts;
};
typedef struct GXMesh_s GXMesh_t;

// Constructors
GXMesh_t    *createMesh       ( );                                        // ✅ Creates an empty mesh
GXPart_t    *createPart       ( );                                        // ✅ Creates an empty part

// Loaders
GXMesh_t    *loadMesh         ( const char   path[] );                    // ✅ Loads a mesh from the disk
GXMesh_t    *loadMeshAsJSON   ( char        *token );                     // ✅ Loads a mesh from a JSON file

GXPart_t    *loadPart         ( const char   path[] );                    // ✅ Loads a part from the disk
GXPart_t    *loadPartAsJSON   ( char        *token );                     // ✅ Loads a part from a JSON file

// Getters
GXPart_t    *getPart          ( GXMesh_t    *mesh, const char  name[] );  // ✅ Finds a part in a mesh

// Appenders
int          appendPart       ( GXMesh_t    *mesh, GXPart_t   *part );    // ✅ Appends a part to a mesh. Updates the count.

// Drawing
int          drawMesh         ( GXMesh_t    *mesh, GXShader_t *shader );  // ✅ Draws a mesh with shader
int          drawPart         ( GXPart_t    *part );                      // ✅ Draws a part with the current shader program

// Removers
int          removePart       ( GXMesh_t    *mesh, const char  name [] ); // ✅ Removes a part from a mesh by name. Updates the count. 

// Destructors
int          destroyMesh      ( GXMesh_t    *mesh );                      // ✅ Destroy a mesh and all of its contents
int          destroyPart      ( GXPart_t    *part );                      // ✅ Destroys a part and all of its contents
