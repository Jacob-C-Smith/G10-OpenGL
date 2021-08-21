﻿#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/GXtypedef.h>
#include <G10/GXEntity.h>
#include <G10/GXCamera.h>
#include <G10/GXLight.h>

#include <G10/GXPhysics.h>
#include <G10/GXCollider.h>

#include <G10/arch/x86_64/GXAVXmath.h>

#include <G10/GXSkybox.h>
#include <G10/GXTexture.h>

// Contains information about a scene
struct GXScene_s {
    char        *name;
    GXEntity_t  *entities;
    GXCamera_t  *cameras;
    GXLight_t   *lights;
    GXSkybox_t  *skybox;
};
typedef struct GXScene_s GXScene_t;

// Constructors
GXScene_t  *createScene         ( );                                      // ✅ Create a scene with nothing in it
GXScene_t  *loadScene           ( const char path[] );                    // ✅ Loads a scene from JSON file at path
GXScene_t  *loadSceneAsJSON     ( char      *token  );

// Appenders
int         appendEntity        ( GXScene_t *scene, GXEntity_t *entity ); // ✅ Add an entity to the end of an entity list
int         appendCamera        ( GXScene_t *scene, GXCamera_t *camera ); // ✅ Appends a camera to the end of a camera list
int         appendLight         ( GXScene_t *scene, GXLight_t  *light  ); // ✅ Appends a light to the end of a camera list

// Drawing
int         drawScene           ( GXScene_t *scene );                     // ✅ Iterates through the scene, drawing all valid entities where the GXEdrawable flag == 1

// Physicas 
int         computePhysics      ( GXScene_t *scene, float deltaTime );

// Getters + Setters
GXEntity_t *getEntity           ( GXScene_t *scene, const char name[] );  // ✅ Searches for the specified entity and returns a pointer to it, else returns nullptr.
GXCamera_t *getCamera           ( GXScene_t *scene, const char name[] );  // ✅ Searches for the specified camera and returns a pointer to it, else returns nullptr.
GXLight_t  *getLight            ( GXScene_t *scene, const char name[] );  // ✅ Searches for the specified light and returns a pointer to it, else returns nullptr.

int         setActiveCamera     ( GXScene_t *scene, const char name[] );  // ✅ Finds and sets the active camera

// Removers
GXEntity_t *removeEntity        ( GXScene_t *scene, const char name[] );  // ✅ Remove an entity by name
GXCamera_t *removeCamera        ( GXScene_t *scene, const char name[] );  // ✅ Removes a camera by name
GXLight_t  *removeLight         ( GXScene_t *scene, const char name[] );  // ✅ Removes a light by name

// Destructors
int         destroyScene        ( GXScene_t *scene );                     // ✅ Destroy a scene and all of its contents