#pragma once

#include <G10/GXDebug.h>
#include <G10/GXtypedef.h>
#include <G10/GXEntity.h>
#include <G10/GXCamera.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Contains information about a scene
struct GXscene_s {
	GXentity_t* head;
	GXcamera_t* camera;
};
typedef struct GXscene_s GXscene_t;

GXscene_t*  createScene  ( );                                      // ✅ Create a scene with nothing in it
GXscene_t*  loadScene    ( const char path[] );                    // ✅ Loads a scene from JSON file at path
int         appendEntity ( GXscene_t* scene, GXentity_t* entity ); // ✅ Add an entity to the end of the linked list
int         drawScene    ( GXscene_t* scene );                     // ✅ Iterates through the scene, drawing all valid entities where the GXEdrawable flag == 1
GXentity_t* getEntity    ( GXscene_t* scene, const char name[] );  // ✅ Searches for the specified entity and returns a pointer to it, else returns nullptr.
int         removeEntity ( GXscene_t* scene, const char name[] );  // 💤 Remove an entity by name
int         destroyScene ( GXscene_t* scene );                     // ✅ destroy a scene and all of its contents