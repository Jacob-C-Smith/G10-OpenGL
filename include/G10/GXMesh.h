#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>
#include <G10/GXJSON.h>

#include <G10/GXTransform.h>
#include <G10/GXMaterial.h>

// Stores information about a part of a mesh
struct GXPart_s
{
    // Name
    char            *name;

    // Material
    const char      *material;

    // Local transform
    GXTransform_t   *transform;    

    // Vertex groups
    size_t           vertexGroups;

    // Array
    GLuint           vertexArray;

    // Geometric vertecies
    GLuint           vertexBuffer;
    GLuint           elementBuffer;

    GLuint           elementsInBuffer;

    struct GXPart_s *next;
};

// Constructors
GXPart_t    *createPart       ( void );                                   // ✅ Creates an empty part

// Loaders
GXPart_t    *loadPart         ( const char   path[] );                    // ✅ Loads a part from the disk
GXPart_t    *loadPartAsJSON   ( char        *token );                     // ✅ Loads a part from a JSON file

// Getters
GXPart_t    *getPart          ( GXPart_t    *head, const char  name[] );  // ✅ Finds a part in a mesh

// Appenders
int          appendPart       ( GXPart_t    *head, GXPart_t   *part );    // ✅ Appends a part to a mesh. Updates the count.

// Drawing
int          drawPart         ( GXPart_t    *part );                      // ✅ Draws a part with the current shader program

// Removers
GXPart_t *   removePart       (GXPart_t     *head, const char  name [] ); // ✅ Removes a part from a mesh by name. Updates the count. 

// Destructors
int          destroyPart      ( GXPart_t    *part );                      // ✅ Destroys a part and all of its contents
