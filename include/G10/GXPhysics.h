#pragma once

#include <stdio.h>
#include <string.h>
#include <stdio.h>

#include <G10/GXtypedef.h>
#include <G10/GXScene.h>
#include <G10/GXLinear.h>

GXvec3_t* summateForces         ( GXvec3_t   *forces, size_t      forceCount); // 
int       integrateDisplacement ( GXEntity_t *entity, float       deltaTime);  // 
bool      collision             ( GXEntity_t *a,      GXEntity_t *b );         // 
int       resolve               ( GXEntity_t *a,      GXEntity_t *b );         // 