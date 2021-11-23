#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/GXtypedef.h>
#include <G10/G10.h>
#include <G10/GXJSON.h>

#include <G10/GXLinear.h>

#include <G10/GXMesh.h>
#include <G10/GXShader.h>
#include <G10/GXTransform.h>
#include <G10/GXMaterial.h>
#include <G10/GXRigidbody.h>
#include <G10/GXCollider.h>

struct GXEntity_s
{
    char              *name;      // A name for the entitiy
    GXPart_t          *parts;     // A list of parts
    GXShader_t        *shader;    // A shader
    GXMaterial_t      *materials; // A list of materials
    GXTransform_t     *transform; // A transform
    GXRigidbody_t     *rigidbody; // A rigidbody
    GXCollider_t      *collider;  // A collider

    struct GXEntity_s *next;      // Points to the next entity.
};

GXEntity_t *createEntity     ( void );                                     // ✅ Creates an entity, assigns flags, and returns pointer to it

GXEntity_t *loadEntity       ( const char  path[] );                       // ✅ Loads an entity from the JSON file at path; Automatically populates it according to the JSON file.
GXEntity_t *loadEntityAsJSON ( char       *token  );                       // ✅ Loads an entity as a JSON object

int         drawEntity       ( GXEntity_t *entity );                       // ✅ Draws an entity if draw flag is set

int         destroyEntity    ( GXEntity_t *entity );                       // ✅ Destroys the entity and all its contents