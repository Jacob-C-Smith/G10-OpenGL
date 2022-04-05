#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <G10/GXtypedef.h>

#include <JSON/JSON.h>

#include <G10/GXEntity.h>
#include <G10/GXCamera.h>
#include <G10/GXLight.h>
#include <G10/GXPhysics.h>
#include <G10/GXLinear.h>
#include <G10/GXSkybox.h>
#include <G10/GXGJK.h>
#include <G10/GXBVH.h>
#include <G10/GXQuaternion.h>

#include <G10/arch/x86_64/GXAVXmath.h>

// Contains information about a scene
struct GXScene_s {
    char             *name;
    GXEntity_t       *entities;
    GXCamera_t       *cameras;
    GXLight_t        *lights;
    GXBV_t           *bvh;
    GXCollision_t    *collisions;
    GXSkybox_t       *skybox;

    struct GXScene_s *next;
};

// Allocators
GXScene_t     *create_scene       ( void );                                 // ✅ Create a scene with nothing in it

// Constructors
GXScene_t     *load_scene         ( const char path[] );                    // ✅ Loads a scene from JSON file at path
GXScene_t     *load_scene_as_json ( char      *token  );

// Appenders
int            append_entity      ( GXScene_t *scene, GXEntity_t    *entity ); // ✅ Appends an entity to the end of the entity list
int            append_camera      ( GXScene_t *scene, GXCamera_t    *camera ); // ✅ Appends a camera to the end of the camera list
int            append_light       ( GXScene_t *scene, GXLight_t     *light  ); // ✅ Appends a light to the end of the camera list
int            append_collision   ( GXScene_t *scene, GXCollision_t *collision );

// Drawing
int            draw_scene         ( GXScene_t *scene );               
int            draw_lights        ( GXScene_t *scene, GXPart_t      *light_part, GXShader_t* shader ); // Draws lights

// Physics 
int            compute_physics    ( GXScene_t *scene, float          delta_time );

// Accessors
GXEntity_t    *get_entity         ( GXScene_t *scene, const char     name[] );  // ✅ Searches for the specified entity and returns a pointer to it, else returns nullptr.
GXCamera_t    *get_camera         ( GXScene_t *scene, const char     name[] );  // ✅ Searches for the specified camera and returns a pointer to it, else returns nullptr.
GXLight_t     *get_light          ( GXScene_t *scene, const char     name[] );  // ✅ Searches for the specified light and returns a pointer to it, else returns nullptr.

// Mutators
int            set_active_camera  ( GXScene_t *scene, const char     name[] );  // ✅ Finds and sets the active camera
 
// Removers
GXEntity_t    *remove_entity      ( GXScene_t *scene, const char     name[] );  // ✅ Removes an entity by name
GXCamera_t    *remove_camera      ( GXScene_t *scene, const char     name[] );  // ✅ Removes a camera by name
GXLight_t     *remove_light       ( GXScene_t *scene, const char     name[] );  // ✅ Removes a light by name
GXCollision_t *remove_collision   ( GXScene_t *scene, GXCollision_t *collision );

// Destructors
int            destroy_scene      ( GXScene_t *scene );                         // ✅ Destroy a scene and all of its contents