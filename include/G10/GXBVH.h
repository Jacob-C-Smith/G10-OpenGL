#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>

#include <G10/GXLinear.h>
#include <G10/GXScene.h>
#include <G10/GXEntity.h>
#include <G10/GXQuaternion.h>

struct GXBV_s
{
    vec3          *location,
                  *dimensions;
    GXEntity_t    *entity;
    struct GXBV_s *left,
                  *right;
};

// Allocators
GXBV_t *create_bv             ( void );                                       // Creates an empty bounding volume

// Constructors
GXBV_t *create_bv_from_entity ( GXEntity_t *entity );                         // Creates a bounding volume from the dimensions and location of an entity
GXBV_t *create_bv_from_bvs    ( GXBV_t     *a     , GXBV_t      *b );  // 
GXBV_t *create_bvh_from_scene ( GXScene_t  *scene );                          // 

float   distance              ( GXBV_t *a, GXBV_t *b );

// Tree navigation
GXBV_t *find_closest_bv       ( GXBV_t     *bvh   , GXBV_t      *bv );
GXBV_t *find_parent_bv        ( GXBV_t     *bvh   , GXBV_t      *bv );

// Tree resizing
int     resize_bv             ( GXBV_t     *bv );

// Debugging
int     print_bv              ( FILE       *f     , GXBV_t     *bv       , size_t      d );

// Getters
size_t  get_entities_from_bv  ( GXBV_t     *bv    , GXEntity_t **entities, size_t      count );

// Drawing functions
int     draw_bv               ( GXBV_t     *bv    , GXPart_t    *cube    , GXShader_t *shader, GXCamera_t *camera, int depth ); // Draws the bounding volume and all its childern in different colors
int     draw_scene_bv         ( GXScene_t  *scene , GXPart_t    *cube    , GXShader_t *shader, int depth );                     // Draws the entire BVH at one time

// Collision detection
bool    checkIntersection     ( GXBV_t      *a    , GXBV_t      *b );  // 

// Destructors
int     destroy_bv            ( GXBV_t      *bv ); // Destroyes a bounding volume and all its childern
