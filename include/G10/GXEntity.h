#pragma once

#include <stdlib.h>

#include <G10/GXtypedef.h>

#include <G10/GXLinear.h>

#include <G10/GXMesh.h>
#include <G10/GXPLY.h>

#include <G10/GXTexture.h>
#include <G10/GXBitmap.h>
#include <G10/GXJPG.h>
#include <G10/GXPNG.h>

#include <G10/GXShader.h>

#include <G10/GXCamera.h>

#include <G10/GXQuaternion.h>
#include <G10/GXilmath.h>
#include <G10/GXTransform.h>

#include <G10/GXJSON.h>

#include <G10/GXMaterial.h>

#include <G10/GXRigidbody.h>

#include <G10/GXCollider.h>

#include <G10/arch/x86_64/GXSSEmath.h>

// G10 uses bitflags to recognize the features of an entity, as well as how it should be interfaced with by the engine

// General flags
#define GXEFPresent          0x0000000000000001 // If set, the object is valid

// Renderer flags
#define GXEFMesh             0x0000000000000002 // If set, mesh is present
#define GXEFShader           0x0000000000000004 // If set, shader is present
#define GXEFMaterial         0x0000000000000008 // If set, PBR material is present
#define GXEFTransform        0x0000000000000010 // If set, transform is present
#define GXEFDrawable         0x0000000000000020 // If set, render on update cycle
#define GXEFDynamic          0x0000000000000400 // If set, the objects position will be changing quite often. Dynamic and static bits are mutually exculsive
#define GXEFStatic           0x0000000000000800 // If set, the objects position will not be changing. Bits 10 and 11 are mutually exclusive

// Physics flags
#define GXEFRigidbody        0x0000000000000040 // If set, use rigid body physics
#define GXEFComputesPhysics  0x0000000000000080 // If set, compute physics on update cycle
#define GXEFCollisionFixed   0x0000000000000100 // If set, use fixed collision detection
#define GXEFCollisionDynamic 0x0000000000000200 // If set, use dynamic collision detection

#define GXEFStandardObject GXEFPresent | GXEFMesh | GXEFShader | GXEFMaterial | GXEFTransform | GXEFDrawable

struct GXEntity_s
{
    size_t             flags;     // Tells G10 how to handle the entity
    char*              name;      // An identifier for the entity
    GXMesh_t*          mesh;      // A mesh
    GXShader_t*        shader;    // A shader
    GXMaterial_t*      material;  // A PBR material
    GXTransform_t*     transform; // A transform
    GXRigidbody_t*     rigidbody; // A rigidbody
    GXCollider_t*      collider;  // A collider

    struct GXEntity_s* next;      // Points to the next entity.
};
typedef struct GXEntity_s GXEntity_t;

GXEntity_t* createEntity     ( );                                          // ✅ Creates an entity, assigns flags, and returns pointer to it
int         drawEntity       ( GXEntity_t* entity );                       // ✅ Draws an entity if draw flag is set
GXEntity_t* loadEntity       ( const char path[] );                        // ✅ Loads an entity from the JSON file at path; Automatically populates it according to the JSON file.
GXEntity_t* loadEntityAsJSON ( char* token );                              // ✅ Loads an entity as a JSON object
int         assignTexture    ( GXShader_t* shader, const char uniform[] ); // ✅ Assigns a texture to a glsl uniform
int         destroyEntity    ( GXEntity_t* entity );                       // ✅ Destroys the entity and all its contents