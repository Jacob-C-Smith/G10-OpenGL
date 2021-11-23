#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>

#include <G10/GXLinear.h>
#include <G10/GXScene.h>
#include <G10/GXEntity.h>

struct GXBV_s
{
    vec3          *location,
                  *scale;
    GXEntity_t    *entity;
    struct GXBV_s *left,
                  *right;
};

GXBV_t *createBV           ( void );                                   // Creates an empty bounding volume
GXBV_t *createBVFromEntity ( GXEntity_t *entity );                     // Creates a bounding volume from the dimensions and location of an entity
GXBV_t *createBVFromBVs    ( GXBV_t     *a             , GXBV_t *b );  // 
GXBV_t *createBVHFromScene ( GXScene_t  *scene );                      // 

GXBV_t *findClosestBV      ( GXBV_t     *bvh           , GXBV_t *bv );

int     printBV            ( GXBV_t     *bv            , size_t d );

int     drawBV             ( GXBV_t     *bv            , GXPart_t* cube, GXShader_t* shader, GXCamera_t *camera, int depth ); // Draws the bounding volume and all its childern in different colors
int     drawSceneBV        ( GXScene_t  *scene         , GXPart_t* cube, GXShader_t* shader, int depth );                     // Draws the entire BVH at one time

bool    checkIntersection  ( GXBV_t      *a            , GXBV_t *b );  // 


