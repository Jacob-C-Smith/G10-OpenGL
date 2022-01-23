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
                  *scale;
    GXEntity_t    *entity;
    struct GXBV_s *left,
                  *right;
};

GXBV_t *create_bv             ( void );                                       // Creates an empty bounding volume
GXBV_t *create_bv_from_entity ( GXEntity_t *entity );                         // Creates a bounding volume from the dimensions and location of an entity
GXBV_t *create_bv_from_bvs    ( GXBV_t     *a     , GXBV_t      *b );  // 
GXBV_t *create_bvh_from_scene ( GXScene_t  *scene );                          // 

GXBV_t *find_closest_bv       ( GXBV_t     *bvh   , GXBV_t      *bv );
GXBV_t *find_parent_bv        ( GXBV_t     *bvh   , GXBV_t      *bv );

int     print_bv              ( GXBV_t     *bv    , size_t       d );

size_t  get_entities_from_bv  ( GXBV_t     *bv    , GXEntity_t **entities, size_t      count );

int     draw_bv               ( GXBV_t     *bv    , GXPart_t    *cube    , GXShader_t *shader, GXCamera_t *camera, int depth ); // Draws the bounding volume and all its childern in different colors
int     draw_scene_bv         ( GXScene_t  *scene , GXPart_t    *cube    , GXShader_t *shader, int depth );                     // Draws the entire BVH at one time

bool    checkIntersection     ( GXBV_t      *a    , GXBV_t      *b );  // 


