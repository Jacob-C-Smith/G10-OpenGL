#pragma once
#include <G10/GXDebug.h>

#include <G10/GXtypedef.h>
#include <G10/GXLinear.h>

#include <G10/GXMesh.h>
#include <G10/GXOBJ.h>
#include <G10/GXPLY.h>

#include <G10/GXTexture.h>
#include <G10/GXBitmap.h>
#include <G10/GXPNG.h>
#include <G10/GXJPG.h>

#include <G10/GXShader.h>

#include <G10/GXCamera.h>

#include <G10/GXQuaternion.h>
#include <G10/GXilmath.h>

#include <G10/GXJSON.h>

#define GXE_present         0x00000001 // If not set, ignore
#define GXE_mesh            0x00000002 // If set, mesh present
#define GXE_shader          0x00000004 // If set, shader present
#define GXE_texture         0x00000008 // If set, texture present
#define GXE_drawable        0x00000010 // If set, draw on update

#define GXE_PMSTD GXE_present | GXE_mesh | GXE_shader | GXE_texture | GXE_drawable

struct GXEntity_s
{
	GXsize_t           flags;  // Tells G10 how to handle the entity
	char*              name;   // An identifier for the entity
	GXmesh_t*          mesh;   // A mesh
	GXshader_t*        shader; // A shader
	GXtexture_t*       UV;     // A UV image
	// Model matrix stuff
	GXvec3_t           location;
	GXvec3_t           rotation;
	GXvec3_t           scale;
	GXmat4_t           model;

	struct GXEntity_s* next;  // Points to the next entity.
};
typedef struct GXEntity_s GXentity_t;

GXentity_t* createEntity  (GXsize_t flags);                           // ✅ Creates an entity, assigns flags, and returns pointer to it
int         drawEntity    (GXentity_t* entity);                       // ✅ Draws an entity if draw flag is set
GXentity_t* loadEntity    (const char path[]);                        // ✅ Loads an entity from the JSON file at path; Automatically populates it according to the JSON file.
int         assignTexture (GXshader_t* shader, const char uniform[]); // ✅ Assigns a texture to a glsl uniform
int         destroyEntity (GXentity_t* entity);                       // ✅ Destroys the entity and all its contents