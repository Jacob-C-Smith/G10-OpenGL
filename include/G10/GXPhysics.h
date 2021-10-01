#pragma once

#include <stdio.h>
#include <string.h>
#include <stdio.h>

#include <G10/GXtypedef.h>
#include <G10/GXLinear.h>

#include <G10/GXEntity.h>

vec3     *summateForces         ( vec3       *forces, size_t      forceCount); // 
int       integrateDisplacement ( GXEntity_t *entity, float       deltaTime);  // 
bool      collision             ( GXEntity_t *a,      GXEntity_t *b );         // 
int       resolve               ( GXEntity_t *a,      GXEntity_t *b );         // 