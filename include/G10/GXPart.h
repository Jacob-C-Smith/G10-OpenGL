﻿#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>
#include <JSON/JSON.h>

#include <G10/GXTransform.h>
#include <G10/GXMaterial.h>

// Stores information about a part of a mesh
struct GXPart_s
{
    // Name
    char            *name;

    // Material
    char            *material;

    // Local transform
    GXTransform_t   *transform;    

    // Vertex groups
    size_t           vertex_groups;

    // Array
    GLuint           vertex_array;

    // Geometric vertecies
    GLuint           vertex_buffer;
    GLuint           element_buffer;

    GLuint           elements_in_buffer;

    size_t           users;

    struct GXPart_s *next;
};

// Constructors
GXPart_t    *create_part       ( void );                                   // ✅ Creates an empty part

// Constructors
GXPart_t    *load_part         ( const char   path[] );                    // ✅ Loads a part from the disk
GXPart_t    *load_part_as_json ( char        *token );                     // ✅ Loads a part from a JSON file
GXPart_t    *duplicate_part    ( GXPart_t    *part );

// Getters
GXPart_t    *get_part          ( GXPart_t    *head, const char  name[] );  // ✅ Finds a part in a mesh

// Appenders
int          append_part       ( GXPart_t    *head, GXPart_t   *part );    // ✅ Appends a part to a mesh. Updates the count.

// Drawing
int          draw_part         ( GXPart_t    *part );                      // ✅ Draws a part with the current shader program

// Removers
GXPart_t *   remove_part       (GXPart_t     *head, const char  name [] ); // ✅ Removes a part from a mesh by name. Updates the count. 

// Destructors
int          destroy_part      ( GXPart_t    *part );                      // ✅ Destroys a part and all of its contents
