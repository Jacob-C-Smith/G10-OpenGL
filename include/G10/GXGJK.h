#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <G10/G10.h>
#include <G10/GXCollider.h>
#include <G10/GXLinear.h>

vec3 find_extreme_point  ( GXCollider_t  *collider,  vec3 direction );

vec3 support             ( GXCollision_t *collision, vec3 direction );

bool GJK                 ( GXCollision_t *collision );

bool next_simplex        ( vec3          *points,    vec3 direction );

bool same_direction      ( vec3           a,         vec3 b );

bool simplex_line        ( vec3          *points,    vec3 direction );

bool simplex_triangle    ( vec3          *points,    vec3 direction );

bool simplex_tetrahedron ( vec3          *points,    vec3 direction );